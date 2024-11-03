#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/parseutils.h"
#include <libavcodec/codec.h>
#include <libavcodec/packet.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libavutil/log.h>
#include <libavutil/rational.h>
#include <time.h>

int writePacketCount = 0;
int encodeVideo(AVCodecContext *encoderCtx, AVFrame *frame, AVPacket *packet, FILE *dest_fp)
{
    int ret = avcodec_send_frame(encoderCtx, frame);
    // TODO : 处理错误
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "send frame error:%s\n", av_err2str(ret));
        return -1;
    }
    while (ret >= 0)
    {
        avcodec_receive_packet(encoderCtx, packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            return 0;
        }
        else if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "encoder frrame failed:%s\n", av_err2str(ret));
            return -1;
        }
        fwrite(packet->data, 1, packet->size, dest_fp);
        writePacketCount++;
        av_log(NULL, AV_LOG_INFO, "writePacketCount : %d\n", writePacketCount);
        av_packet_unref(packet);
    }
    return 0;
}

int main(int argc, char **argv)
{
    av_log_set_level(AV_LOG_INFO);
    if (argc < 5)
    {
        av_log(NULL, AV_LOG_ERROR, "Usage: %s <inFile> <outFile> <encodeName> <width x height>\n",
               argv[0]);
        return -1;
    }
    const char *inFileName = argv[1];
    const char *outFileName = argv[2];
    const char *encoderName = argv[3];
    int width = 0, height = 0;
    int ret = av_parse_video_size(&width, &height, argv[4]);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR,
               "Invalid size '%s', must be in the form WxH or a valid size abbreviation\n",
               argv[4]);
        return -1;
    }
    enum AVPixelFormat pixFmt = AV_PIX_FMT_YUV420P;
    int fps = 30;
    const AVCodec *encoder = avcodec_find_encoder_by_name(encoderName);
    if (encoder == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "find encoder %s failed\n", encoderName);
        return -1;
    }
    AVCodecContext *encoderCtx = avcodec_alloc_context3(encoder);
    if (encoderCtx == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "alloc encoder context failed!\n");
        return -1;
    }
    encoderCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    encoderCtx->pix_fmt = pixFmt;
    encoderCtx->width = width;
    encoderCtx->height = height;
    encoderCtx->time_base = (AVRational){1, fps};
    encoderCtx->bit_rate = 4096000;
    encoderCtx->max_b_frames = 0;
    encoderCtx->gop_size = 10;
    ret = avcodec_open2(encoderCtx, encoder, NULL);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "open encoder failed! %s\n", av_err2str(ret));
        goto end;
    }
    FILE *src_fp = fopen(inFileName, "rb");
    if (src_fp == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "open infilename error");
        ret = -1;
        goto end;
    }
    FILE *dest_fp = fopen(outFileName, "wb");
    if (dest_fp == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "open outfilename error");
        ret = -1;
        goto end;
    }
    AVFrame *frame = av_frame_alloc();
    int frameSize = av_image_get_buffer_size(pixFmt, width, height, 1);
    uint8_t *frameBuffer = av_malloc(frameSize);
    av_image_fill_arrays(frame->data, frame->linesize, frameBuffer, pixFmt, width, height, 1);
    int pictureSize = width * height;
    AVPacket *packet = av_packet_alloc();
    int readFrameCount = 0;
    while (fread(frameBuffer, 1, pictureSize * 3 / 2, src_fp) == pictureSize * 3 / 2)
    {
        // Y 1 U 1/4 V 1/4
        frame->data[0] = frameBuffer;
        frame->data[1] = frameBuffer + pictureSize;
        frame->data[2] = frameBuffer + pictureSize + pictureSize / 4;
        readFrameCount++;
        av_log(NULL, AV_LOG_INFO, "readFrameCount: %d\n", readFrameCount);
        encodeVideo(encoderCtx, frame, packet, dest_fp);
    }
    encodeVideo(encoderCtx, NULL, packet, dest_fp);

end:
    if (encoderCtx)
    {
        avcodec_free_context(&encoderCtx);
    }
    if (src_fp)
    {
        fclose(src_fp);
    }
    if (dest_fp)
    {
        fclose(dest_fp);
    }
    if (frameBuffer)
    {
        av_freep(&frameBuffer);
    }
    return ret;
}
