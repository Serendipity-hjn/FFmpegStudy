#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include <libavutil/log.h>
#include <time.h>
#include "libavdevice/avdevice.h"

// 显示可用的摄像头设备
void avfoundationListDevices()
{
    const AVInputFormat * inFmt = av_find_input_format("dshow");
    if (inFmt == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "find dshow failed!\n");
    }
    AVFormatContext *inFmtCtx = avformat_alloc_context();
    // 第二个参数是URL
    int ret = avformat_open_input(&inFmtCtx, "dummy", inFmt, NULL);
    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "open input format failed:%s\n", av_err2str(ret));
        return;
    }
}
int main(int argc, char *argv[])
{
    av_log_set_level(AV_LOG_INFO);
    avdevice_register_all();
    avfoundationListDevices();
    return 0;    
}