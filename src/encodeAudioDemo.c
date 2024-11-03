#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include <libavcodec/codec.h>
#include <libavcodec/packet.h>
#include <libavutil/channel_layout.h>
#include <libavutil/error.h>
#include <libavutil/log.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
#include <stdio.h>
#include <time.h>

int encodeAudio(AVCodecContext *encoderCtx, AVFrame *frame, AVPacket *packet, FILE *dest_fp)
{
    int ret = avcodec_send_frame(encoderCtx, frame);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "send frame to encoder failed:%s\n", av_err2str(ret));
        return -1;
    }
    while (ret >= 0)
    {
        ret = avcodec_receive_packet(encoderCtx, packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            return 0;
        }
        else if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "receive packet from encoder failed:%s\n", av_err2str(ret));
            return -1;
        }
        fwrite(packet->data, 1, packet->size, dest_fp);
        av_packet_unref(packet);
    }
    return 0;
}

int main(int argc, char **argv)
{
    av_log_set_level(AV_LOG_INFO);
    if (argc < 3)
    {
        av_log(NULL, AV_LOG_ERROR, "Usage: %s <input file> <output file>\n", argv[0]);
        return -1;
    }
    const char *inFileName = argv[1];
    const char *outFileName = argv[2];

    AVFrame *frame = av_frame_alloc();
    if (!frame)
    {
        av_log(NULL, AV_LOG_ERROR, "Could not allocate video frame\n");
        return -1;
    }

    frame->sample_rate = 44100;
    // 这里代码有些不同
    frame->ch_layout.nb_channels = 2;
    av_channel_layout_from_mask(&frame->ch_layout, AV_CH_LAYOUT_STEREO);
    frame->format = AV_SAMPLE_FMT_S16;
    frame->nb_samples = 1024;

    av_frame_get_buffer(frame, 0);

    int ret = 0;
    const AVCodec *encoder = avcodec_find_encoder_by_name("libfdk_aac");
    if (!encoder)
    {
        av_log(NULL, AV_LOG_ERROR, "find encoder failed\n");
        ret = -1;
        goto end;
    }

    AVCodecContext *encoderCtx = avcodec_alloc_context3(encoder);
    if (!encoderCtx)
    {
        av_log(NULL, AV_LOG_ERROR, "alloc encoder context failed\n");
        ret = -1;
        goto end;
    }

    encoderCtx->sample_fmt = frame->format;
    encoderCtx->sample_rate = frame->sample_rate;
    encoderCtx->ch_layout.nb_channels = frame->ch_layout.nb_channels;
    encoderCtx->ch_layout = frame->ch_layout;

    ret = avcodec_open2(encoderCtx, encoder, NULL);
    if (ret < 0)
    {
        av_log(NULL,AV_LOG_ERROR,"open encoder failed:%s\n",av_err2str(ret));
        ret = -1;
        goto end;
    }

    FILE *src_fp = fopen(inFileName, "rb");
    if (src_fp == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "open input file failed\n");
        ret = -1;
        goto end;
    }

    FILE *dest_fp = fopen(outFileName, "wb+");
    if (src_fp == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "open output file failed\n");
        ret = -1;
        goto end;
    }

    AVPacket *packet = av_packet_alloc();
    
    while (1)
    {
        int readSize = fread(frame->data[0], 1, frame->linesize[0], src_fp);
        if (readSize == 0)
        {
            av_log(NULL, AV_LOG_ERROR, "finish read infile\n");
            break;
        }
        encodeAudio(encoderCtx, frame, packet, dest_fp);
    }
    encodeAudio(encoderCtx, NULL, packet, dest_fp);

end:
    if (frame)
    {
        av_frame_free(&frame);
    }
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
    return ret;
}