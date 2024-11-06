#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include <libavutil/dict.h>
#include <libavutil/log.h>
#include <time.h>
#include "libavdevice/avdevice.h"

// 显示可用的摄像头设备
// ffmpeg -f dshow -list_devices true -i dummy
void avfoundationListDevices()
{
    const AVInputFormat * inFmt = av_find_input_format("dshow");
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
    av_log(NULL,AV_LOG_INFO,"open input format success!\n");
}
int main(int argc, char *argv[])
{
    av_log_set_level(AV_LOG_INFO);
    avdevice_register_all();
    avfoundationListDevices();
    return 0;    
}