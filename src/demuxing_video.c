#include <libavutil/avutil.h>
#include <libavformat/avformat.h>

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

    ret = avformat_find_stream_info(inFmtCtx, NULL);
    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Find stream info failed:%s\n", av_err2str(ret));
        ret = -1;
        goto fail;
    }

    ret = av_find_best_stream(inFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);


    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Find best stream failed:%s\n", av_err2str(ret));
        ret = -1;
        goto fail;
    }

    int videoIndex = ret;

    FILE *dest_fp = fopen(outFilename, "wb");

    if (dest_fp == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "Open output file failed:%s\n", outFilename);
        ret = -1;
        goto fail;

    }

    AVPacket *packet = av_packet_alloc();

    while (av_read_frame(inFmtCtx, packet) == 0)
    {
        if (packet->stream_index == videoIndex)
        {
            int writeSize = fwrite(packet->data, 1, packet->size, dest_fp);
            if (writeSize != packet->size)
            {
                // 这里不能释放整个packet，只能释放packet中的data，因为循环之后还会用到packet
                av_packet_unref(packet);
                ret = -1;
                break;
            }
        }
        av_packet_free(&packet);
    }

    fclose(dest_fp);
fail:
    if(inFmtCtx != NULL)
    {
        avformat_close_input(&inFmtCtx);
    }
    if(dest_fp != NULL)
    {
        fclose(dest_fp);
    }
    return ret;
}