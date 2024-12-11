#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include <libavcodec/packet.h>
#include <libavutil/dict.h>
#include <libavutil/frame.h>
#include <libavutil/log.h>
#include <stdio.h>
#include <time.h>
// 显示可用的摄像头设备
// ffmpeg -f dshow -list_devices true -i dummy
void dshowListDevices()
{

    const AVInputFormat *inFmt = av_find_input_format("dshow");
    if (inFmt == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "find dshow failed!\n");
    }
    // 设置参数
    AVDictionary *options = NULL;
    av_dict_set(&options, "list_devices", "true", 0);

    AVFormatContext *inFmtCtx = avformat_alloc_context();
    // 第二个参数是URL
    int ret = avformat_open_input(&inFmtCtx, "video=Integrated Camera", inFmt, &options);

    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "open input format failed:%s\n", av_err2str(ret));
        return;
    }
    if (inFmtCtx)
    {
        avformat_close_input(&inFmtCtx);
        avformat_free_context(inFmtCtx);
    }
}

void decodeAudio(AVCodecContext *decoderCtx, AVPacket *packet, FILE *dest_fp)
{
    if (avcodec_send_packet(decoderCtx, packet) == 0)
    {
        AVFrame *frame = av_frame_alloc();
        while (avcodec_receive_frame(decoderCtx, frame) >= 0)
        {
            fwrite(frame->data[0], 1, frame->linesize[0], dest_fp);
        }
        av_frame_free(&frame);
    }
}

int main(int argc, char *argv[])
{
    av_log_set_level(AV_LOG_INFO);
    if (argc < 2)
    {
        av_log(NULL, AV_LOG_ERROR, "Usage:%s <outFileName> \n", argv[0]);
        return -1;
    }

    const char *outFileName = argv[1];
    avdevice_register_all();
    dshowListDevices();

    AVFormatContext *inFmtCtx = avformat_alloc_context();
    const AVInputFormat *inFmt = av_find_input_format("dshow");

    if (inFmt == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "open input format failed!\n");
        goto end;
    }

    AVDictionary *options = NULL;
    // av_dict_set(&options, "framerate", "30", 0);
    int ret =
        avformat_open_input(&inFmtCtx, "audio=阵列麦克风 (AMD Audio Device)", inFmt, &options);
    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "open input failed:%s\n", av_err2str(ret));
        goto end;
    }

    ret = avformat_find_stream_info(inFmtCtx, NULL);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "find stream info failed:%s\n", av_err2str(ret));
        goto end;
    }

    ret = av_find_best_stream(inFmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "find best stream failed:%s\n", av_err2str(ret));
        goto end;
    }
    int audioIndex = ret;

    // 创建解码器上下文
    AVCodecContext *decoderCtx = avcodec_alloc_context3(NULL);

    ret = avcodec_parameters_to_context(decoderCtx, inFmtCtx->streams[audioIndex]->codecpar);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "copy parameters to context failed:%s\n", av_err2str(ret));
        goto end;
    }

    const AVCodec *decoder = avcodec_find_decoder(decoderCtx->codec_id);
    if (decoder == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "find decoder failed!\n");
        ret = -1;
        goto end;
    }

    ret = avcodec_open2(decoderCtx, decoder, NULL);
    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "open decoder failed:%s\n", av_err2str(ret));
        goto end;
    }

#if 0
    AVFrame *destFrame = av_frame_alloc();
    enum AVPixelFormat destPixFmt = AV_PIX_FMT_YUV420P;

    uint8_t *outBuffer =
        av_malloc(av_image_get_buffer_size(destPixFmt, decoderCtx->width, decoderCtx->height, 1));
    av_image_fill_arrays(destFrame->data, destFrame->linesize, outBuffer, destPixFmt,
                         decoderCtx->width, decoderCtx->height, 1);

    struct SwsContext *swsCtx =
        sws_getContext(decoderCtx->coded_width, decoderCtx->coded_height, decoderCtx->pix_fmt,
                       decoderCtx->width, decoderCtx->height, destPixFmt, 0, NULL, NULL, NULL);
    if (swsCtx == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "create sws context failed!\n");
        ret = -1;
        goto end;
    }
#endif

    FILE *dest_fp = fopen(outFileName, "wb+");
    if (dest_fp == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "open out put file %s failed!\n", outFileName);
        ret = -1;
        goto end;
    }
    AVPacket *packet = av_packet_alloc();

    while (1)
    {
        if (av_read_frame(inFmtCtx, packet) == 0)
        {
            if (packet->stream_index == audioIndex)
            {
                // decodeVideo(swsCtx, decoderCtx, destFrame, packet, dest_fp);
                decodeAudio(decoderCtx, packet, dest_fp);
            }
        }
        av_packet_unref(packet);
    }
    // decodeVideo(swsCtx, decoderCtx, destFrame, NULL, dest_fp);
    decodeAudio(decoderCtx, NULL, dest_fp);

end:
    if (inFmtCtx)
    {
        avformat_free_context(inFmtCtx);
    }
    if (decoderCtx)
    {
        avcodec_free_context(&decoderCtx);
    }
    if (dest_fp)
    {
        fclose(dest_fp);
    }
    return ret;
}