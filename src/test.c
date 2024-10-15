#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>

int main(int argc, char **argv)
{
    av_log_set_level(AV_LOG_DEBUG);
    if (argc < 3)
    {
        av_log(NULL, AV_LOG_ERROR, "Usage: %s <input> <output>\n", argv[0]);
        return -1;
    }
    const char *inFilename = argv[1];
    const char *outFilename = argv[2];

    AVFormatContext *inFmtCtx = NULL;

    int ret = avformat_open_input(&inFmtCtx, inFilename, NULL, NULL);
    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Open input format failed:%s\n", av_err2str(ret));
        return -1;
    }
    // test

    ret = avformat_find_stream_info(inFmtCtx, NULL);
    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Find stream info failed:%s\n", av_err2str(ret));
        avformat_close_input(&inFmtCtx);
        return -1;
    }

    ret = av_find_best_stream(inFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Find best stream failed:%s\n", av_err2str(ret));
        avformat_close_input(&inFmtCtx);
        return -1;
    }

    int videoIndex = ret;
    AVStream *videoStream = inFmtCtx->streams[videoIndex];
    AVCodecParameters *codecParams = videoStream->codecpar;

    FILE *dest_fp = fopen(outFilename, "wb");
    if (dest_fp == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "Open output file failed:%s\n", outFilename);
        avformat_close_input(&inFmtCtx);
        return -1;
    }

    AVPacket *packet = av_packet_alloc();
    if (!packet)
    {
        av_log(NULL, AV_LOG_ERROR, "Could not allocate packet\n");
        fclose(dest_fp);
        avformat_close_input(&inFmtCtx);
        return -1;
    }

    // 提取并写入编解码器参数
    if (codecParams->codec_id == AV_CODEC_ID_H264)
    {
        AVBitStreamFilterContext *bsf = av_bitstream_filter_init("h264_mp4toannexb");
        if (!bsf)
        {
            av_log(NULL, AV_LOG_ERROR, "Could not initialize bitstream filter\n");
            av_packet_free(&packet);
            fclose(dest_fp);
            avformat_close_input(&inFmtCtx);
            return -1;
        }

        AVPacket filteredPacket;
        av_init_packet(&filteredPacket);
        filteredPacket.data = NULL;
        filteredPacket.size = 0;

        ret = av_bitstream_filter_filter(bsf, videoStream->codec, NULL, &filteredPacket.data, &filteredPacket.size, NULL, 0, 0);
        if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "Bitstream filter failed\n");
            av_bitstream_filter_close(bsf);
            av_packet_free(&packet);
            fclose(dest_fp);
            avformat_close_input(&inFmtCtx);
            return -1;
        }

        if (filteredPacket.size > 0)
        {
            fwrite(filteredPacket.data, 1, filteredPacket.size, dest_fp);
            av_free(filteredPacket.data);
        }

        av_bitstream_filter_close(bsf);
    }

    while (av_read_frame(inFmtCtx, packet) == 0)
    {
        if (packet->stream_index == videoIndex)
        {
            int writeSize = fwrite(packet->data, 1, packet->size, dest_fp);
            if (writeSize != packet->size)
            {
                av_log(NULL, AV_LOG_ERROR, "Failed to write packet data\n");
                av_packet_unref(packet);
                break;
            }
        }
        av_packet_unref(packet);
    }

    av_packet_free(&packet);
    fclose(dest_fp);
    avformat_close_input(&inFmtCtx);
    // test

    return 0;
}
