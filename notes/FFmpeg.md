# 基础命令

[CSDN博主总结常用命令](https://blog.csdn.net/wenmingzheng/article/details/88373192?ops_request_misc=%257B%2522request%255Fid%2522%253A%25228EA3F7CC-D940-48CA-A46F-A57216709319%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=8EA3F7CC-D940-48CA-A46F-A57216709319&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-88373192-null-null.142^v100^pc_search_result_base1&utm_term=ffmpeg%E5%91%BD%E4%BB%A4&spm=1018.2226.3001.4187)

# 获得基础信息，输出Metadata

打开媒体文件，获取Meta信息，关闭媒体文件
## 代码
[dumpMetaData.c](src/dumpMetaData.c)

```c
#include "libavutil/log.h"
#include "libavformat/avformat.h"

// 传入命令行参数个数
int main(int argc, char **argv)
{
	// 设置日志级别
	av_log_set_level(AV_LOG_DEBUG);
	// 设置日志输出函数

	// 检查参数个数
	if (argc < 2)
	{
		av_log(NULL, AV_LOG_DEBUG, "Usage:%s infileName.\n", argv[0]);
		return -1;
	}

	// 获取输入文件名
	const char *infileName = argv[1];

	// 初始化所有组件
	AVFormatContext *pFormatCtx = NULL;

	// 打开媒体文件
	int ret = avformat_open_input(&pFormatCtx, infileName, NULL, NULL);

	// av_err2str()函数返回错误信息
	if (ret != 0)
	{
		// av_err2str()函数返回错误信息
		av_log(NULL, AV_LOG_DEBUG, "open input file:%s failed: %s\n", infileName, av_err2str(ret));
		return -1;
	}

	// 获取媒体文件信息
	av_dump_format(pFormatCtx, 0, infileName, 0);

	// 关闭媒体文件
	avformat_close_input(&pFormatCtx);

	return 0;
}


```

1. 容器/文件 (Container/File)

- **定义**: 特定格式的多媒体文件，如 `.mp4`, `.flv`, `.mov` 等。
- **作用**: 存储和组织多媒体数据，包括音频、视频、字幕等。
- **常见格式**:
    - **MP4**: 广泛用于视频存储和流媒体。
    - **FLV**: 主要用于Flash视频。
    - **MOV**: 苹果公司开发的视频格式。

2. 媒体流 (Stream)

- **定义**: 一段连续的数据，如一段声音数据、一段视频或者一段字幕数据。
- **特点**: 由不同编码器编码。
- **类型**:
    - **音频流**: 存储音频数据。
    - **视频流**: 存储视频数据。
    - **字幕流**: 存储字幕数据。

3. 数据包 (Packet)

- **定义**: 一个媒体流由大量的数据包组成，是压缩后的数据。
- **作用**: 传输和存储媒体数据的基本单位。
- **特点**: 数据包是压缩后的数据，便于传输和存储。

4. 数据帧 (Frame)

- **定义**: 一个数据包由一个或多个数据帧组成，是非压缩数据。
- **作用**: 原始的、未压缩的媒体数据。
- **类型**:
    - **I帧 (Intra Frame)**: 独立帧，不依赖其他帧。
    - **P帧 (Predictive Frame)**: 依赖前一帧进行预测。
    - **B帧 (Bidirectional Frame)**: 依赖前后帧进行预测。

5. 编解码器 (Codec)

- **定义**: 编解码器是以帧为单位实现压缩数据和原始数据之间相互转换的工具。
- **作用**: 用于压缩和解压缩媒体数据。
- **常见编解码器**:
    - **视频编解码器**: H.264, H.265, VP9 等。
    - **音频编解码器**: AAC, MP3, Vorbis 等。

## 重要结构体

- **AVFormatContext**: 管理整个多媒体文件的格式和结构。
- **AVStream**: 表示媒体文件中的一个单独的媒体流。
- **AVCodecContext 与 AVCodec**: 管理媒体数据的编码和解码过程。
- **AVPacket**: 表示压缩后的媒体数据。
- **AVFrame**: 表示未压缩的原始媒体数据。

# 解封装-提取aac数据

AAC（Advanced Audio Coding）是一种高级音频编码技术，广泛用于数字音频压缩和传输。它是由MPEG（Moving Picture Experts Group）开发的，旨在提供比MP3更高的音质和更高的压缩效率。AAC通常用于各种音频应用，包括音乐、视频、广播和流媒体服务。

**AAC的主要特点：**

1. **高音质**：AAC能够在较低的比特率下提供比MP3更高的音质。
2. **多通道支持**：AAC支持多通道音频，包括立体声、5.1环绕声和7.1环绕声。
3. **低延迟**：AAC设计用于低延迟应用，适合实时音频传输。
4. **灵活性**：AAC支持多种比特率和采样率，适用于不同的应用场景。


```bash
ffmpeg -y -i out.mp4 -vn -acodec copy out.aac
ffplay out.aac
```

## 流程
| 操作步骤           | 函数名                  |
|--------------------|-------------------------|
| 打开媒体文件       | `avformat_open_input`    |
| 获取码流信息       | `avformat_find_stream_info` |
| 获取音频流         | `av_find_best_stream`    |
| 初始化 packet      | `av_packet_alloc`         |
| 读取 packet 数据   | `av_read_frame`          |
| 释放 packet 数据   | `av_packet_unref`        |
| 关闭媒体文件       | `avformat_close_input`   |

## 代码
[demuxing_audio.c](src/demuxing_audio.c)
```c
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"

int main(int argc, char *argv[])
{
    // 设置日志级别
    av_log_set_level(AV_LOG_DEBUG);

    // 如果参数小于3，输出使用方法
    if (argc < 3)
    {
        // argv[0]是程序名
        av_log(NULL, AV_LOG_ERROR, "Usage: %s <input file> <output file>\n", argv[0]);
        return -1;
    }

    // 获取命令行的输入音频
    const char *inputName = argv[1];
    // 获取命令行的输出音频
    const char *outputName = argv[2];

    av_sdp_create;
    // 打开输入音频文件
    AVFormatContext *inFormatCtx = NULL;

    // 打开媒体文件，并获取流信息
    int ret = avformat_open_input(&inFormatCtx, inputName, NULL, NULL);
    // 如果打开输入文件失败，返回错误信息
    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Could not open input file '%s'\n", inputName);
        return -1;
    }

    // 获取码流信息
    ret = avformat_find_stream_info(inFormatCtx, NULL);

    // 如果ret小于0，则打印错误信息
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "find stream info failed:%s\n", av_err2str(ret));
        // 就算获取失败，也要关闭输入文件
        avformat_close_input(&inFormatCtx);
        return -1;
    }

    // 如果获取成功，则打印信息
    int audioIndex = av_find_best_stream(inFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audioIndex < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Could not find audio stream in the input file\n");
        avformat_close_input(&inFormatCtx);
        return -1;
    }

    if (audioIndex < 0)
    {
        // 输出错误信息，表示找不到最佳音频流
        av_log(NULL, AV_LOG_ERROR, "find best stream failed, index is %d\n", audioIndex);
        avformat_close_input(&inFormatCtx);
        return -1;
    }

    // 打印音频信息
    av_log(NULL, AV_LOG_INFO, "the audio index is %d\n", audioIndex);

    // 初始化AVPacket结构体
    AVPacket *packet = av_packet_alloc();
    if (!packet)
    {
        av_log(NULL, AV_LOG_ERROR, "Could not allocate packet\n");
        avformat_close_input(&inFormatCtx);
        return -1;
    }

    // 存储音频流信息 输出文件
    FILE *dest_fp = fopen(outputName, "wb");

    if (dest_fp == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "open %s file failed\n", outputName);
        // 就算打不开文件也得关闭音频文件
        avformat_close_input(&inFormatCtx);
        // 释放分配的AVPacket
        av_packet_free(&packet);
        return -1;
    }

    // 有许多PC数据，所以需要循环读取
    while (av_read_frame(inFormatCtx, packet) == 0)
    {
        // 检查当前包是否属于音频流
        if (packet->stream_index == audioIndex)
        {
            // 将音频数据写入输出文件
            fwrite(packet->data, 1, packet->size, dest_fp);
            // 检查写入是否成功
            if (ret != packet->size)
            {
                // 如果写入的数据大小不等于包的大小，则输出错误信息
                av_log(NULL, AV_LOG_ERROR, "write data failed\n");
                // 关闭输出文件
                fclose(dest_fp);
                // 关闭输入文件
                avformat_close_input(&inFormatCtx);
                // 释放整个结构体
                av_packet_free(&packet);
                return -1;
            }
        }
        // 释放当前包的引用
        av_packet_unref(packet);
    }

    // 检查输入格式上下文是否已初始化
    if (inFormatCtx != NULL)
    {
        // 关闭输入文件
        avformat_close_input(&inFormatCtx);
    }
    // 检查输出文件指针是否已初始化
    if (dest_fp != NULL)
    {
        // 关闭输出文件
        fclose(dest_fp);
    }
    if (packet != NULL)
    {
        // 释放AVPacket结构体
        av_packet_free(&packet);
    }

    return 0;
}

```

## aac音频格式分析

ADTS（Audio Data Transport Stream）和ADIF（Audio Data Interchange Format）是两种用于音频编码的容器格式，主要用于AAC（Advanced Audio Codec）音频编码。它们的主要区别在于数据流的组织方式和使用场景。

### ADTS（Audio Data Transport Stream）

- **定义**: ADTS是一种流式传输格式，适用于音频数据的实时传输，如广播、流媒体等。
- **结构**: 每个ADTS帧都包含一个头信息，后面跟着音频数据。头信息中包含了帧的长度、采样率、声道数等信息。
- **特点**:
    - **自包含**: 每个ADTS帧都是自包含的，可以独立解码。
    - **流式传输**: 适合流式传输，因为每个帧都可以独立处理。
    - **头部信息**: 每个帧的头部信息较大，可能会增加一些开销。

### ADIF（Audio Data Interchange Format）

- **定义**: ADIF是一种文件格式，适用于音频数据的存储和交换，如音频文件的存储。
- **结构**: ADIF文件包含一个唯一的头信息，后面跟着所有的音频数据。头信息中包含了编码参数、采样率、声道数等信息。
- **特点**:
    - **单一头部**: 整个文件只有一个头部信息，减少了冗余。
    - **非流式**: 不适合流式传输，因为需要整个文件的头信息才能开始解码。
    - **存储和交换**: 适合存储和交换音频数据，因为头部信息只出现一次，减少了文件大小。

### 总结

- **ADTS**: 适用于流式传输，每个帧自包含，适合实时传输。
- **ADIF**: 适用于文件存储和交换，整个文件只有一个头部信息，适合存储和交换音频数据。

选择哪种格式取决于具体的应用场景：如果需要实时传输音频数据，ADTS是更好的选择；如果需要存储或交换音频文件，ADIF更为合适。

# 提取H264视频数据

## 流程
流程和提取aac文件一样
| 操作步骤           | 函数名                  |
|--------------------|-------------------------|
| 打开媒体文件       | `avformat_open_input`    |
| 获取码流信息       | `avformat_find_stream_info` |
| 获取音频流         | `av_find_best_stream`    |
| 初始化 packet      | `av_packet_alloc`         |
| 读取 packet 数据   | `av_read_frame`          |
| 释放 packet 数据   | `av_packet_unref`        |
| 关闭媒体文件       | `avformat_close_input`   |

## 代码
[demuxing_video.c](src/demuxing_video.c)
```c
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
```

成功运行，要用avi格式的视频文件

如果想提取mp4格式的文件，需要进行以下步骤

# mp4→h264
## 流程
| 函数名                  | 描述                                                         |
|-------------------------|--------------------------------------------------------------|
| `av_bsf_get_by_name`    | 根据名称获取比特流过滤器                                     |
| `av_bsf_alloc`          | 分配比特流过滤器上下文                                       |
| `avcodec_parameters_copy` | 复制编解码器参数                                             |
| `av_bsf_init`           | 初始化比特流过滤器                                           |
| `av_bsf_send_packet`    | 发送数据包到比特流过滤器                                     |
| `av_bsf_receive_packet` | 从比特流过滤器接收处理后的数据包                             |
| `av_bsf_free`           | 释放比特流过滤器上下文及相关资源                             |

## 代码
[mp4toh264.c](src/mp4toh264.c)
```c
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libavcodec/bsf.h>

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

    FILE *dest_fp = fopen(outFilename, "wb+");

    if (dest_fp == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "Open output file failed:%s\n", outFilename);
        ret = -1;
        goto fail;

    }

    AVPacket *packet = av_packet_alloc();

    const AVBitStreamFilter *bsf = av_bsf_get_by_name("h264_mp4toannexb");
    if(bsf == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "get h264_mp4toannexb bsf failed\n");
        ret = -1;
        goto fail;
    }
    
    AVBSFContext *bsfCtx = NULL;
    av_bsf_alloc(bsf, &bsfCtx);
    avcodec_parameters_copy(bsfCtx->par_in, inFmtCtx->streams[videoIndex]->codecpar);
    av_bsf_init(bsfCtx);

    while (av_read_frame(inFmtCtx, packet) == 0)
    {
        if (packet->stream_index == videoIndex)
        {
            if(av_bsf_send_packet(bsfCtx, packet) == 0)
            {
                while(av_bsf_receive_packet(bsfCtx, packet) == 0)
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
    if(bsfCtx != NULL)
    {
        av_bsf_free(&bsfCtx); 
    }
    if(dest_fp != NULL)
    {
        fclose(dest_fp);
    }
    return ret;
}
```

# 转封装-mp4转flv

## I帧，P帧，B帧

I帧：帧内编码帧（Intra picture），I帧通常是一个GOP的第一帧，经过轻度地压缩，作为随机访问的参考点，可以当成静态图像，I帧压缩可去掉视频的空间冗余信息。

P帧：前向预测编码帧（predictive frame），通过将图像序列中前面已编码帧的时间冗余信息充分去除来压缩传输数据量的编码图像，也称为预测帧。

B帧：双向预测内插编码帧，既考虑源图像序列前面的已编码帧，又顾及源图像序列后面的已编码帧之间的时间冗余信息，来压缩传输数据量的编码图像，也称为双向预测帧

PTS-显示时间戳

DTS-解码时间戳

## 流程

| 步骤                   | 对应函数                       |
| ---------------------- | ------------------------------ |
| 打开输入媒体文件       | `avformat_open_input`            |
| 获取输入流信息         | `avformat_find_stream_info`      |
| 创建输出流上下文       | `avformat_alloc_output_context2` |
| 创建输出码流的AVStream | `avformat_new_stream`            |
| 拷贝编码参数           | `avcodec_parameters_copy`        |
| 写入视频文件头         | `avformat_write_header`          |
| 读取输入视频流         | `av_read_frame`                  |
| 计算pts/dts/duration   | `av_rescale_q_rnd`/`av_rescale_q`  |
| 写入视频流数据         | `av_interleaved_write_frame`     |
| 写入视频文件末尾       | `av_write_trailer`               |

## 代码
[mp4toflv.c](src/mp4toflv.c)
```c
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>

int main(int argc, char **argv)
{
    av_log_set_level(AV_LOG_DEBUG);
    if (argc < 3)
    {
        av_log(NULL, AV_LOG_ERROR, "Usage: %s <infileName> <outfileName>\n", argv[0]);
        return -1;
    }
    const char *inFileName = argv[1];
    const char *outFileName = argv[2];

    AVFormatContext *inFmtCtx = NULL;
    int ret = avformat_open_input(&inFmtCtx, inFileName, NULL, NULL);
    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "open input format failed:%s\n", av_err2str(ret));
        return -1;
    }

    ret = avformat_find_stream_info(inFmtCtx, NULL);
    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "find input stream failed:%s\n", av_err2str(ret));
        goto fail;
    }

    AVFormatContext *outFmtCtx = NULL;

    // 分配输出格式上下文
    ret = avformat_alloc_output_context2(&outFmtCtx, NULL, NULL, outFileName);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "alloc output format failed:%s\n", av_err2str(ret));
        goto fail;
    }
    // 输入文件的流数量
    int streamCount = inFmtCtx->nb_streams;
    // 分配一个整数数组，用于存储输入流索引到输出流索引的映射关系，并将其初始化为零
    int *handleStreamIndexArray = av_malloc_array(streamCount, sizeof(int));
    if (handleStreamIndexArray == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "malloc handle stream index array failed\n");
        goto fail;
    }  

    int streamIndex = 0;

    // 用于多媒体处理的循环，主要功能是将输入文件中的音视频流复制到输出文件中
    for (int i = 0; i < streamCount; i++)
    {
        // 获取输入文件的流
        AVStream *inStream = inFmtCtx->streams[i];
        // 判断流的类型（视频，音频或字幕）
        if (inStream->codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
            inStream->codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
            inStream->codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE)
        {
            // 不处理该流
            handleStreamIndexArray[i] = -1;
            continue;
        }
        handleStreamIndexArray[i] = streamIndex++;
        // 创建新的输出流
        AVStream *outStream = NULL;
        // 在输出文件中创建一个新的流
        outStream = avformat_new_stream(outFmtCtx, NULL);
        if (outStream == NULL)
        {
            ret = -1;
            av_log(NULL, AV_LOG_ERROR, "new output stream failed\n");
            goto fail;
        }
        // 复制编解码器参数
        avcodec_parameters_copy(outStream->codecpar, inStream->codecpar);
        // 设置输出流的编解码器标签为0
        outStream->codecpar->codec_tag = 0;
    }

    // 判断outFmtCtx->oformat->flags是否包含AVFMT_NOFILE标志
	  [&解释（点击跳转）](https://www.notion.so/if-outFmtCtx-oformat-flags-AVFMT_NOFILE-1187c25c79d08036bde1c286d0b3c943?pvs=21)
    if (!(outFmtCtx->oformat->flags & AVFMT_NOFILE))
    {
        // 以写入模式打开
        ret = avio_open(&outFmtCtx->pb, outFileName, AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "open output file failed:%s\n", outFileName);
            goto fail;
        }
    }

    // 将输出文件的头部信息写入到输出文件中
    ret = avformat_write_header(outFmtCtx, NULL);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "write header failed:%s\n", av_err2str(ret));
        goto fail;
    }
    AVPacket *packet = av_packet_alloc();

    // 读取输入文件的数据包
    while (av_read_frame(inFmtCtx, packet) == 0)
    {
        if (packet->stream_index >= streamCount ||
            handleStreamIndexArray[packet->stream_index == -1])
        {
            av_packet_unref(packet);
        }

        // 获取输入输出文件中对应流索引的流
        AVStream *inStream = inFmtCtx->streams[packet->stream_index];
        AVStream *outStream = outFmtCtx->streams[packet->stream_index];

        packet->stream_index = handleStreamIndexArray[packet->stream_index];
        packet->pts = av_rescale_q(packet->pts, inStream->time_base, outStream->time_base);
        packet->dts = av_rescale_q(packet->dts, inStream->time_base, outStream->time_base);
        packet->duration = av_rescale_q(packet->duration, inStream->time_base, outStream->time_base);
        // 将数据包的位置设置为-1
        packet->pos = -1;

        ret = av_interleaved_write_frame(outFmtCtx, packet);
        if (ret != 0)
        {
            av_log(NULL, AV_LOG_ERROR, "write interleaved failed:%s\n", av_err2str(ret));
            goto fail;
        }

        av_packet_unref(packet);
    }
    ret = av_write_trailer(outFmtCtx);
    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "write trailer failed :%s\n", av_err2str(ret));
    }
fail:
    if (inFmtCtx)
    {
        avformat_close_input(&inFmtCtx);
    }
    if (outFmtCtx && !(outFmtCtx->oformat->flags & AVFMT_NOFILE))
    {
        avio_closep(&outFmtCtx->pb);
    }
    if (outFmtCtx)
    {
        avformat_free_context(outFmtCtx);
    }
    if (handleStreamIndexArray)
    {
        av_freep(&handleStreamIndexArray);
    }
    return ret;
}
```

# 截取封装文件

## 时间基与时间戳

时间基：时间刻度，表示每个刻度多少秒（就像一把尺子的刻度）

时间戳：表示占多少个时间刻度，单位不是秒，而是时间刻度（多少多少cm）

时间基和时间戳相乘就是时间

PTS：显示时间戳，在什么时候开始显示这一帧数据，转成时间：PTS * 时间基

DTS：解码时间戳，在什么时候开始解码这一帧数据，转成时间：DTS * 时间基

## 流程

截取封装文件处理流程和转封装流程几乎一样，只是多了一个跳转指定时间戳的步骤。以下是详细流程：

| 步骤                      | 对应函数                       |
| ------------------------- | ------------------------------ |
| 1. 打开输入媒体文件       | `avformat_open_input`            |
| 2. 获取输入流信息         | `avformat_find_stream_info`      |
| 3. 创建输出流上下文       | `avformat_alloc_output_context2` |
| 4. 创建输出码流的AVStream | `avformat_new_stream`            |
| 5. 拷贝编码参数           | `avcodec_parameters_copy`        |
| 6. 写入视频文件头         | `avformat_write_header`          |
| 7. 读取输入视频流         | `av_read_frame`                  |
| 8. 跳转指定时间戳         | `av_seek_frame`                  |
| 9. 计算pts/dts/duration   | `av_rescale_q_rnd`/`av_rescale_q`  |
| 10. 写入视频流数据        | `av_interleaved_write_frame`     |
| 11. 写入视频文件末尾      | `av_write_trailer`               |

## 代码
[demuxing_dir.c](src/demuxing_dir.c)

```c
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

int main(int argc, char **argv)
{
    // 设置日志级别
    av_log_set_level(AV_LOG_INFO);
    if (argc < 2)
    {
        av_log(NULL, AV_LOG_ERROR, "Usage:%s <infileName>\n", argv[0]);
    }
    const char *inFileName = argv[1];

    // 打开输入文件
    AVFormatContext *inFmtCtx = NULL;                       // 用于存储输入文件的格式信息
    avformat_open_input(&inFmtCtx, inFileName, NULL, NULL); // 打开输入文件inFileName，并将格式信息存储在inFmtCtx中

    avformat_find_stream_info(inFmtCtx, NULL); // 查找输入文件的流信息，并将流信息存储在inFmtCtx中

    av_dump_format(inFmtCtx, 0, inFileName, 0); // 打印输入文件inFileName的格式信息

    av_log(NULL, AV_LOG_INFO, "input file duration:%ld us, %lf s \n", inFmtCtx->duration, inFmtCtx->duration * av_q2d(AV_TIME_BASE_Q)); // 打印输入文件的总时长，单位为微秒和秒 AV_TIME_BASE_Q是ffmpeg内部的时间基，值为{1, AV_TIME_BASE}，AV_TIME_BASE的值为1000000，即1秒

    // AVRational是ffmpeg内部的时间基，值为{num, den}，num为分子，den为分母
    AVRational videoTimeBase;
    AVRational audioTimeBase;
    for (int i = 0; i < inFmtCtx->nb_streams; i++) // 遍历输入文件中的所有流
    {
        AVStream *inStream = inFmtCtx->streams[i]; // 获取输入文件中的第i个流
        // 分别判断是否为音频或视频流
        if (inStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoTimeBase = inStream->time_base;
            av_log(NULL, AV_LOG_INFO, "video timebase:num = %d,den = %d\n", videoTimeBase.num, videoTimeBase.den);
        }
        else if (inStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audioTimeBase = inStream->time_base;
            av_log(NULL, AV_LOG_INFO, "audio timebase:num = %d,den = %d\n", audioTimeBase.num, audioTimeBase.den);
        }
    }
    AVPacket *packet = av_packet_alloc();        // 分配一个AVPacket结构体，用于存储解码后的数据
    while (av_read_frame(inFmtCtx, packet) >= 0) // 循环读取输入文件中的每个数据包，并将数据包存储在packet中
    {
        AVStream *inStream = inFmtCtx->streams[packet->stream_index]; // 获取当前数据包所属的流
        av_log(NULL, AV_LOG_INFO, "streamIndex = %d,pts = %ld,ptsTime = %lf,dts = %ld,dtsTime = %lf\n", packet->stream_index, packet->pts, packet->pts * av_q2d(inStream->time_base), packet->dts, packet->dts * av_q2d(inStream->time_base)); // 打印当前数据包的流索引、pts、pts时间、dts、dts时间
    }
    return 0;
}
```

# 视频解码
如何使用ffmpeg接口对视频解码
## RGB介绍
**三原色**：RGB色彩模式是工业界的一种颜色标准，是通过对红(R)、绿(G)、蓝(B)三个颜色通道的变化以及它们相互之间的叠加来得到各式各样的颜色的，RGB即是代表红、绿、蓝三个通道的颜色，这个标准几乎包括了人类视力所能感知的所有颜色，是目前运用最广的颜色系统之一。

**显示器**：使用RGB三种颜色的发光体作为基本发光单元

**分辨率**：手机屏幕分辨率是1280`*`720，表示屏幕上有1280`*`720个像素点，每个像素点由RGB三种颜色组成
### RGB格式
**调色版**：通过编号映射到颜色的一张二维表，如01索引，表示红色
**索引格式**： 
RGB1、RGB4、RGB8 是计算机图形学中常见的颜色编码格式，它们代表了不同的颜色深度和存储方式。以下是对这些格式的解释：

1. **RGB1**：
   - **颜色深度**：1位（bit）。
   - **颜色数量**：2种颜色（通常是黑色和白色）。
   - **应用场景**：常用于早期的单色显示器或简单的图形界面，如文本模式下的显示。

2. **RGB4**：
   - **颜色深度**：4位（bit）。
   - **颜色数量**：16种颜色。
   - **应用场景**：常用于早期的彩色显示器或低分辨率图形界面，如早期的计算机游戏或简单的图形应用程序。

3. **RGB8**：
   - **颜色深度**：8位（bit）。
   - **颜色数量**：256种颜色。
   - **应用场景**：常用于早期的彩色显示器或低分辨率图形界面，如早期的计算机游戏、网页设计中的调色板模式等。

这些格式在现代计算机图形处理中已经较少使用，但在某些特定的应用场景或历史研究中仍然具有参考价值。
**像素格式**：。。。（后续觉得有必要再补上）

**命令**

ffmpeg命令将图片转RGB数据
```bash
ffmpeg -i input.png -pix_fmt rgb24 output.rgb
```

注意输出信息中会输出图片大小，下面的`ffplay`需要用 
```text
Stream #0:0: Video: png, rgba(pc, gbr/bt709/iec61966-2-1), 1920x1200 [SAR 5669:5669 DAR 8:5], 25 fps, 25 tbr, 25 tbn
```

ffplay命令播放RGB数据
```bash
ffplay -f output.rgb -pix_fmt rgb24 -s widthxheight output.rgb
```

其中，`width` 和 `height` 是图片的宽度和高度，是必要的信息。

通过解码，会发现照片内存明显变大，因为RGB格式存储了更多的颜色信息，所以我们需要对照片进行编码

TODO完善
## YUV介绍 
YUV 是一种颜色编码系统，常用于视频和图像处理中。`Y` 代表亮度（Luminance），`U` 和 `V` 代表色度（Chrominance）。YUV 格式有多种变体，如 YUV420、YUV422、YUV444 等。

## 流程
| 函数名                          | 描述                                   |
| ------------------------------- | -------------------------------------- |
| `av_find_best_stream`           | 在媒体文件中查找最佳流                 |
| `avcodec_alloc_context3`        | 分配一个编解码器上下文                 |
| `avcodec_parameters_to_context` | 复制编解码器参数                       |
| `avcodec_find_decoder`          | 查找并获取视频解码器                   |
| `avcodec_open2`                 | 打开解码器上下文，并与指定的解码器关联 |
| `av_read_frame`                 | 读取帧                                 |
| `avcodec_send_packet`           | 发送数据包到解码器                     |
| `avcodec_receive_frame`         | 从解码器接收帧                         |

输入指令
```bash
./demoBin ../video/test.mp4 test.yuv
ffplay test.yuv -video_size 720x1280 -pixel_format yuv420p 
```

如果播放的视频乱码，主要是由于`width`和`linesize`大小不一样
后续的更改视频格式的时候会解决这个问题

## 代码
[decodeVideo.c](src/decodeVideo.c)
```c
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"

// 定义一个全局变量，用于记录解码的帧数
int frameCount = 0;

// 解码视频帧的函数
int decodeVideo(AVCodecContext *codecCtx, AVPacket *packet, FILE *dest_fp)
{
    // 将数据包发送到解码器
    int ret = avcodec_send_packet(codecCtx, packet);
    if (ret != 0)
    {
        // 如果发送失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not send packet:%s\n", av_err2str(ret));
        return -1;
    }

    // 分配一个AVFrame结构体，用于存储解码后的帧数据
    AVFrame *frame = av_frame_alloc();
    // 循环接收解码后的帧数据
    while (avcodec_receive_frame(codecCtx, frame) == 0)
    {
        // 将帧数据写入输出文件
        fwrite(frame->data[0], 1, codecCtx->width * codecCtx->height, dest_fp);
        fwrite(frame->data[1], 1, codecCtx->width * codecCtx->height / 4, dest_fp);
        fwrite(frame->data[2], 1, codecCtx->width * codecCtx->height / 4, dest_fp);
        // 增加帧计数
        frameCount++;
        // 记录当前帧数
        av_log(NULL, AV_LOG_INFO, "frameCount:%d\n", frameCount);
    }
    // 如果帧数据不为空，释放帧内存
    if (frame)
    {
        av_frame_free(&frame);
    }
    return 0;
}

int main(int argc, char **argv)
{
    // 设置日志级别为调试模式
    av_log_set_level(AV_LOG_DEBUG);
    // 检查命令行参数是否正确
    if (argc < 3)
    {
        av_log(NULL, AV_LOG_ERROR, "Usage: %s <input> <output>\n", argv[0]);
        return -1;
    }
    // 获取输入和输出文件名
    const char *inFileName = argv[1];
    const char *outFileName = argv[2];

    // 定义一个AVFormatContext结构体，用于存储输入文件的格式信息
    AVFormatContext *inFmtCtx = NULL;
    // 打开输入文件
    int ret = avformat_open_input(&inFmtCtx, inFileName, NULL, NULL);
    if (ret != 0)
    {
        // 如果打开失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not open input file %s\n", inFileName);
        return -1;
    }

    // 获取输入文件的流信息
    ret = avformat_find_stream_info(inFmtCtx, NULL);
    if (ret < 0)
    {
        // 如果获取失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not find stream information:%s\n", av_err2str(ret));
        goto fail;
    }

    // 查找最佳的视频流索引
    ret = av_find_best_stream(inFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (ret < 0)
    {
        // 如果查找失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not find best stream index:%s\n", av_err2str(ret));
        goto fail;
    }

    // 获取视频流的索引
    int videoIndex = ret;

    // 分配一个AVCodecContext结构体，用于存储解码器上下文信息
    AVCodecContext *codecCtx = avcodec_alloc_context3(NULL);
    if (codecCtx == NULL)
    {
        // 如果分配失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not allocate codec context\n");
        ret = -1;
        goto fail;
    }

    // 将流参数复制到解码器上下文
    avcodec_parameters_to_context(codecCtx, inFmtCtx->streams[videoIndex]->codecpar);

    // 查找解码器
    const AVCodec *decoder = avcodec_find_decoder(codecCtx->codec_id);
    if (decoder == NULL)
    {
        // 如果查找失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not find codec\n");
        ret = -1;
        goto fail;
    }

    // 打开解码器
    ret = avcodec_open2(codecCtx, decoder, NULL);
    if (ret != 0)
    {
        // 如果打开失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not open codec:%s\n", av_err2str(ret));
        goto fail;
    }

    // 打开输出文件
    FILE *dest_fp = fopen(outFileName, "wb+");
    if (dest_fp == NULL)
    {
        // 如果打开失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not open output file %s\n", outFileName);
        ret = -1;
        goto fail;
    }

    // 分配一个AVPacket结构体，用于存储数据包
    AVPacket *packet = av_packet_alloc();

    // 分配一个AVFrame结构体，用于存储解码后的帧数据
    AVFrame *frame = av_frame_alloc();
    // 循环读取输入文件中的数据包
    while (av_read_frame(inFmtCtx, packet) >= 0)
    {
        // 如果数据包属于视频流ff
        if (packet->stream_index == videoIndex)
        {
            // 解码视频帧
            if (decodeVideo(codecCtx, packet, dest_fp) == -1)
            {
                ret = -1;
                av_packet_unref(packet);
                goto fail;
            }

            // 释放数据包引用
            av_packet_unref(packet);
        }
    }
    // 刷新解码器，确保所有帧都被解码
    decodeVideo(codecCtx, NULL, dest_fp);
fail:
    // 如果输入文件格式上下文不为空，关闭输入文件
    if (inFmtCtx)
    {
        avformat_close_input(&inFmtCtx);
    }
    // 如果解码器上下文不为空，释放解码器上下文
    if (codecCtx)
    {
        avcodec_free_context(&codecCtx);
    }
    // 如果输出文件指针不为空，关闭输出文件
    if (dest_fp)
    {
        fclose(dest_fp);
    }
    return ret;
}
```

# 更改视频格式

## 流程
| 函数名                  | 描述                                                                 |
|-------------------------|----------------------------------------------------------------------|
| `av_parse_video_size`   | 解析视频尺寸字符串（如 "1920x1080"）并返回宽度和高度。               |
| `sws_getContext`        | 创建一个 `SwsContext`，用于图像缩放和格式转换。                      |
| `av_frame_alloc`        | 分配一个 `AVFrame` 结构体，用于存储解码后的视频帧。                  |
| `av_image_get_buffer_size` | 计算给定图像格式和尺寸所需的缓冲区大小。                             |
| `av_malloc`             | 分配内存，用于存储图像数据。                                         |
| `av_image_fill_arrays`  | 将图像数据填充到 `AVFrame` 的缓冲区中，并设置相关的行大小和数据指针。 |
| `sws_scale`             | 使用 `SwsContext` 对图像进行缩放或格式转换。                         |

## 代码
[decodeVideoChange.c](src/decodeVideoChange.c)

```c
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/parseutils.h"
#include "libavutil/imgutils.h"

// 定义一个全局变量，用于记录解码的帧数
int frameCount = 0;

// 解码视频帧的函数
int decodeVideo(AVCodecContext *codecCtx, AVPacket *packet, struct SwsContext *swsCtx, int destWidth, int destHeight, AVFrame *destFrame, FILE *dest_fp)
{
    // 将数据包发送到解码器
    int ret = avcodec_send_packet(codecCtx, packet);
    if (ret != 0)
    {
        // 如果发送失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not send packet:%s\n", av_err2str(ret));
        return -1;
    }

    // 分配一个AVFrame结构体，用于存储解码后的帧数据
    AVFrame *frame = av_frame_alloc();
    // 循环接收解码后的帧数据
    while (avcodec_receive_frame(codecCtx, frame) == 0)
    {
        sws_scale(swsCtx, (const uint8_t *const*)frame->data, frame->linesize, 0, codecCtx->height, destFrame->data, destFrame->linesize);
        // 将帧数据写入输出文件
        fwrite(destFrame->data[0], 1, destWidth * destHeight, dest_fp);
        fwrite(destFrame->data[1], 1, destWidth * destHeight / 4, dest_fp);
        fwrite(destFrame->data[2], 1, destWidth * destHeight / 4, dest_fp);
        // 增加帧计数
        frameCount++;
        // 记录当前帧数
        av_log(NULL, AV_LOG_INFO, "frameCount:%d\n", frameCount);
        // 输出宽高信息,linesize0 1 2
        av_log(NULL, AV_LOG_INFO, "width:%d,height:%d,linesize0:%d,linesize1:%d,linesize2:%d\n", destWidth, destHeight, destFrame->linesize[0], destFrame->linesize[1], destFrame->linesize[2]);
    }
    // 如果帧数据不为空，释放帧内存
    if (frame)
    {
        av_frame_free(&frame);
    }
    return 0;
}

int main(int argc, char **argv)
{
    // 设置日志级别为调试模式
    av_log_set_level(AV_LOG_DEBUG);
    // 检查命令行参数是否正确
    if (argc < 4)
    {
        av_log(NULL, AV_LOG_ERROR, "Usage: %s <input> <output> <width*height>\n", argv[0]);
        return -1;
    }
    // 获取输入和输出文件名
    const char *inFileName = argv[1];
    const char *outFileName = argv[2];
    const char *destVideoSizeString = argv[3];
    int destWidth = 0, destHeight = 0;
    int ret = av_parse_video_size(&destWidth, &destHeight, destVideoSizeString);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "invalid video size:%s\n", destVideoSizeString);
        return -1;
    }
    av_log(NULL, AV_LOG_INFO, "destWith:%d,destHeight:%d\n", destWidth, destHeight);

    // 定义一个AVFormatContext结构体，用于存储输入文件的格式信息
    AVFormatContext *inFmtCtx = NULL;
    // 打开输入文件
    ret = avformat_open_input(&inFmtCtx, inFileName, NULL, NULL);
    if (ret != 0)
    {
        // 如果打开失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not open input file %s\n", inFileName);
        return -1;
    }

    // 获取输入文件的流信息
    ret = avformat_find_stream_info(inFmtCtx, NULL);
    if (ret < 0)
    {
        // 如果获取失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not find stream information:%s\n", av_err2str(ret));
        goto fail;
    }

    // 查找最佳的视频流索引
    ret = av_find_best_stream(inFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (ret < 0)
    {
        // 如果查找失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not find best stream index:%s\n", av_err2str(ret));
        goto fail;
    }

    // 获取视频流的索引
    int videoIndex = ret;

    // 分配一个AVCodecContext结构体，用于存储解码器上下文信息
    AVCodecContext *codecCtx = avcodec_alloc_context3(NULL);
    if (codecCtx == NULL)
    {
        // 如果分配失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not allocate codec context\n");
        ret = -1;
        goto fail;
    }

    // 将流参数复制到解码器上下文
    avcodec_parameters_to_context(codecCtx, inFmtCtx->streams[videoIndex]->codecpar);

    // 查找解码器
    const AVCodec *decoder = avcodec_find_decoder(codecCtx->codec_id);
    if (decoder == NULL)
    {
        // 如果查找失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not find codec\n");
        ret = -1;
        goto fail;
    }

    // 打开解码器
    ret = avcodec_open2(codecCtx, decoder, NULL);
    if (ret != 0)
    {
        // 如果打开失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not open codec:%s\n", av_err2str(ret));
        goto fail;
    }

    enum AVPixelFormat destPixfmt = codecCtx->pix_fmt;

    struct SwsContext *swsCtx = sws_getContext(codecCtx->width, codecCtx->height, codecCtx->pix_fmt, destWidth, destHeight, destPixfmt, SWS_BICUBIC, NULL, NULL, NULL);
    if (swsCtx == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "Could not create SwsContext\n");
        ret = -1;
        goto fail;
    }

    AVFrame *destFrame = av_frame_alloc();

    uint8_t *outBuffer = av_malloc(av_image_get_buffer_size(destPixfmt, destWidth, destHeight, 1));
    av_image_fill_arrays(destFrame->data, destFrame->linesize, outBuffer, destPixfmt, destWidth, destHeight, 1);

    // 打开输出文件
    FILE *dest_fp = fopen(outFileName, "wb+");
    if (dest_fp == NULL)
    {
        // 如果打开失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not open output file %s\n", outFileName);
        ret = -1;
        goto fail;
    }

    // 分配一个AVPacket结构体，用于存储数据包
    AVPacket *packet = av_packet_alloc();

    // 分配一个AVFrame结构体，用于存储解码后的帧数据
    AVFrame *frame = av_frame_alloc();
    // 循环读取输入文件中的数据包
    while (av_read_frame(inFmtCtx, packet) >= 0)
    {
        // 如果数据包属于视频流
        if (packet->stream_index == videoIndex)
        {
            // 解码视频帧
            // if (decodeVideo(codecCtx, packet, dest_fp) == -1)
            if (decodeVideo(codecCtx, packet, swsCtx, destWidth, destHeight, destFrame, dest_fp) == -1)
            {
                ret = -1;
                av_packet_unref(packet);
                goto fail;
            }

            // 释放数据包引用
            av_packet_unref(packet);
        }
    }
    // 刷新解码器，确保所有帧都被解码
    // decodeVideo(codecCtx, NULL, dest_fp);
    decodeVideo(codecCtx, NULL, swsCtx, destWidth, destHeight, destFrame, dest_fp);
fail:
    // 如果输入文件格式上下文不为空，关闭输入文件
    if (inFmtCtx)
    {
        avformat_close_input(&inFmtCtx);
    }
    // 如果解码器上下文不为空，释放解码器上下文
    if (codecCtx)
    {
        avcodec_free_context(&codecCtx);
    }
    // 如果输出文件指针不为空，关闭输出文件
    if (dest_fp)
    {
        fclose(dest_fp);
    }
    if (destFrame)
    {
        av_frame_free(&destFrame);
    }
    if (outBuffer)
    {
        av_free(outBuffer);
    }
    return ret;
}
```

## 解码后的数据存储

解码后的视频数据通常存储在 `data[0]`、`data[1]`、`data[2]` 等数组中。具体来说：

- **`data[0]`**: 存储了 `linesize[0] * height` 个数据。
- **`data[1]`** 和 **`data[2]`**: 存储了其他平面的数据（如YUV格式中的U和V平面）。

## 内存对齐和 `linesize`

- **`linesize[0]`**: 实际上并不等于图像的宽度 `width`，而是比宽度大。
- 这种差异是由于内存对齐的需求，以及解码器的CPU和其他优化原因导致的。

## `sws_scale` 函数功能

`sws_scale` 函数是 FFmpeg 中用于图像缩放和格式转换的核心函数。它主要完成以下功能：

1. **图像色彩空间转换**：
   - 将图像从一种色彩空间转换为另一种色彩空间，例如从 RGB 转换为 YUV，或者从 YUV420P 转换为 YUV444P。

2. **分辨率缩放**：
   - 调整图像的分辨率，例如将 1920x1080 的图像缩放到 1280x720。

3. **前后图像滤波处理**：
   - 在进行缩放和色彩空间转换时，应用滤波器以平滑图像，减少锯齿和伪影。

## BMP文件格式
**概念**：BMP文件格式，又称为Bitmap（位图）或是DIB（Device-Independent Device，设备无光位图），是Windows操作系统中的标准图像文件格式。由于它可以不作任何变换地保存图像像素域的数据，因此成为我们取得RAW数据的好来源。

**扫描方式**：从左到右，从下到上

**文件组成**：
- 位图文件头（Bitmap File Header）：提供文件的格式，大小等信息
- 位图信息头（Bitmap Information）：提供图像的尺寸，位平面数，压缩方式，颜色索引等信息。
- 调色板（Color Palette）：可选，有些位图需要调色板，有些位图，比如真彩色图（24位的BMP）就不需要调色板。
- 位图数据（Bitmap Data）：图像数据区

**文件头结构体**：
```cpp
typedef struct tagBITMAPFILEHEADER {
    WORD bfType;                    // 文件类型，必须是0x424D，即字符“BM”
    DWORD bfSize;                   // bmp文件大小
    WORD bfReserved1;               // 保留字
    WORD bfReserved2;               // 保留字
    DWORD bfOffBits;                // 实际位图数据的偏移字节数，即前三个部分长度之和
    } BITMAPFILEHEADER;
```

**信息头结构体**：
```cpp
typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;                   //表示struct tagBITMAPINFOHEADER的长度，设为40
    LONG biWidth;                   //bmp图片宽度
    LONG biHeight;                  //bmp图片高度
    WORD biPlanes;                  //bmp图片平面树，设为1
    WORD biBitCount;                //bmp图片位数，即1位图，4位图，8位图，24位图等
    DWORD biCompression;            //bmp图片压缩类型，0表示不压缩
    DWORD biSizeImage;              //bmp图片数据大小，必须是4的整数倍
    LONG biXPelsPerMeter;           //bmp图片水平分辨率
    LONG biYPelsPerMeter;           //bmp图片垂直分辨率
    DWORD biClrUsed;                //bmp图片实际使用的颜色表中的颜色数
    DWORD biClrImportant;           //bmp图片对显示有重要影响的颜色索引的数目
    } BITMAPINFOHEADER;
```

# 视频编码（yuv到h264）
## 流程
| 函数名                     | 描述               |
| -------------------------- | ------------------ |
| `avcodec_find_encoder`     | 查找编码器         |
| `avcodec_alloc_context3`   | 创建编码器上下文   |
| `avcodec_open2`            | 打开编码器         |
| `av_frame_alloc`           | 分配帧内存         |
| `av_image_get_buffer_size` | 获取图像缓冲区大小 |
| `av_image_fill_arrays`     | 填充图像数据数组   |
| `avcodec_send_frame`       | 发送帧到编码器     |
| `avcodec_receive_packet`   | 从编码器接收数据包 |

## 代码
[encodeVideoDemo.c](src/encodeVideoDemo.c)
```c
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
            av_log(NULL,AV_LOG_ERROR,"encoder frrame failed:%s\n",av_err2str(ret));
            return -1;
        }
        fwrite(packet->data, 1, packet->size, dest_fp);
        writePacketCount++;
        av_log(NULL,AV_LOG_INFO,"writePacketCount : %d\n",writePacketCount);
        av_packet_unref(packet);
    }
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

```
# 音频解码

## PCM介绍
PCM（Pulse Code Modulation）是一种用于数字音频的标准编码格式。它通过将模拟音频信号转换为数字信号来表示音频数据。PCM 编码的基本原理是将模拟音频信号在时间上进行采样，并将每个采样点的幅度值量化为离散的数字值。<br>
核心过程：采样->量化->编码

### PCM关键要素
- 采样率（Sample Rate）：每秒采样的次数，常见的采样率有 44.1 kHz、48 kHz 等。
- 量化格式（Sample Format）：每个采样点的位数，常见的量化格式有 16 位、24 位等。
- 声道数（Channels）：音频信号的声道数，如单声道、立体声等。 

### PCM数据格式
- 存储格式
  - 双声道：采样数据按LRLR方式存储，即左声道和右声道交替存储，存储的时候与字节序有关。
  - 单声道：采样数据按时间顺序存储（有时也会采用LRLR方式，但另一个声道数据为0）。
<img src="imagesForNotes/声道.png" alt =声道 >

- 存储格式分为`Packed`和`Planner`两种，对于双通道音频，`Packed`为两个声道的数据交错存储;`Planner`为两个声道的数据分开存储。
  - `Packed`：LRLRLR
  - `Planner`：LLLRRR 

- ffmpeg音频解码后的数据存放在AVFrame结构体中：
  - Packed格式下，frame.data[0]存放所有声道的数据。
  - Planner格式下，frame.data[i]存放第i个声道的数据。
  - 左声道data[0]:LLLL...
  - 右声道data[1]:RRRR...
- Planner模式是ffmpeg内部存储模式，实际使用的音频文件都是Packed模式。

### PCM计算
- 大小计算：以CD的音质为例：量化格式为16比特（2字节），采样率为44100，声道数为2。
  - 比特率为：16 * 44100 * 2 = 1378.125 kbps
  - 每秒存储空间：1378.125 * 60/8/1024 = 10.09MB
- ffmpeg提取pcm数据命令：
```bash
    ffmpeg -i input.aac -ar 48000 -ac 2 -f s16le output.pcm
```

- ffplay播放pcm数据命令：
```bash
    ffplay -ar 48000 -ac 2 -f s16le output.pcm
```

通过上述指令播放不成功的话，可以尝试转换PCM文件
```bash
ffmpeg -f s16le -ar 48000 -ac 2 -i output.pcm output_stereo.wav
ffplay output_stereo.wav
```

## 流程

| 函数名                        | 描述                                                                 |
|-------------------------------|----------------------------------------------------------------------|
| `avformat_open_input()`        | 打开输入文件或流并读取头部信息。                                       |
| `avformat_find_stream_info()`  | 读取一些数据包以获取流信息。                                           |
| `av_find_best_stream()`        | 查找最佳流（音频、视频或字幕）。                                       |
| `avcodec_alloc_context3()`     | 分配解码器上下文。                                                     |
| `avcodec_parameters_to_context()` | 将流参数复制到解码器上下文中。                                         |
| `avcodec_find_decoder()`       | 查找合适的解码器。                                                     |
| `avcodec_open2()`              | 打开解码器。                                                           |
| `av_frame_alloc()`             | 分配AVFrame结构体。                                                    |
| `av_samples_get_buffer_size()` | 计算音频缓冲区的大小。                                                 |
| `avcodec_fill_audio_frame()`   | 填充音频帧的缓冲区。                                                   |
| `av_read_frame()`              | 从输入文件或流中读取数据包。                                           |
| `avcodec_send_packet()`        | 将数据包发送到解码器进行解码。                                         |
| `avcodec_receive_frame()`      | 从解码器接收解码后的帧。                                               |


## 代码
[decodeAudio.c](src/decodeAudio.c)
```c
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include <libavcodec/codec.h>
#include <libavcodec/packet.h>
#include <time.h>

int decodeAudio(AVCodecContext *decoderCtx, AVPacket *packet, AVFrame *frame, FILE *dest_fp)
{
    int ret = avcodec_send_packet(decoderCtx, packet);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "send packet to decoder failed: %s\n", av_err2str(ret));
        return -1;
    }
    int channel = 0;
    while (ret >= 0)
    {
        ret = avcodec_receive_frame(decoderCtx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            return 0;
        }
        else if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "decode packet failed: %s\n", av_err2str(ret));
            return -1;
        }
        int dataSize = av_get_bytes_per_sample(decoderCtx->sample_fmt);
        if (dataSize < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "get bytes per sample failed\n");
            return -1;
        }
        // frame fltp 2
        /*
            data[0] L L L L
            data[1] R R R R

            --> L R L R L R L R
        */
        for (int i = 0; i < frame->nb_samples; i++)
        {
            for (channel = 0; channel < decoderCtx->ch_layout.nb_channels; channel++)
            {
                fwrite(frame->data[channel] + dataSize * i, 1, dataSize, dest_fp);
            }
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    av_log_set_level(AV_LOG_DEBUG);
    if (argc < 3)
    {
        av_log(NULL, AV_LOG_ERROR, "Usage: %s <input> <output>\n", argv[0]);
    }
    const char *inFileName = argv[1];
    const char *outFileName = argv[2];

    AVFormatContext *inFmtCtx = NULL;

    int ret = avformat_open_input(&inFmtCtx, inFileName, NULL, NULL);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "open %s failed\n", inFileName);
        return -1;
    }

    ret = avformat_find_stream_info(inFmtCtx, NULL);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "find stream error:%s\n", av_err2str(ret));
        goto fail;
    }

    ret = av_find_best_stream(inFmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "find best stream error:%s\n", av_err2str(ret));
        goto fail;
    }

    int audioStreamIndex = ret;
    AVCodecContext *decoderCtx = avcodec_alloc_context3(NULL);
    if (decoderCtx == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "alloc codec context failed\n");
        goto fail;
    }

    ret = avcodec_parameters_to_context(decoderCtx, inFmtCtx->streams[audioStreamIndex]->codecpar);
    const AVCodec *decoder = avcodec_find_decoder(decoderCtx->codec_id);
    if (decoder == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "find decoder %d failed\n", decoderCtx->codec_id);
        ret = -1;
        goto fail;
    }

    ret = avcodec_open2(decoderCtx, decoder, NULL);
    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "open decoder error:%s\n", av_err2str(ret));
        goto fail;
    }

    FILE *dest_fp = fopen(outFileName, "wb");
    if (dest_fp == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "open %s failed\n", outFileName);
        ret = -1;
        goto fail;
    }

    AVFrame *frame = av_frame_alloc();
    int frameSize = av_samples_get_buffer_size(NULL, decoderCtx->ch_layout.nb_channels, frame->nb_samples,
                                               decoderCtx->sample_fmt, 1);
    uint8_t *frameBuffer = av_malloc(frameSize);

    avcodec_fill_audio_frame(frame, decoderCtx->ch_layout.nb_channels, decoderCtx->sample_fmt,
                             frameBuffer, frameSize, 1);

    AVPacket *packet = av_packet_alloc();
    while (av_read_frame(inFmtCtx, packet) >= 0)
    {
        if (packet->stream_index == audioStreamIndex)
        {
            decodeAudio(decoderCtx, packet, frame, dest_fp);
        }
        av_packet_unref(packet);
    }
    decodeAudio(decoderCtx, NULL, frame, dest_fp);

fail:
    if (inFmtCtx)
    {
        avformat_close_input(&inFmtCtx);
    }
    if (decoderCtx)
    {
        avcodec_free_context(&decoderCtx);
    }
    if (frame)
    {
        av_frame_free(&frame);
    }
    if (frameBuffer)
    {
        av_freep(frameBuffer);
    }
    if (dest_fp)
    {
        fclose(dest_fp);
    }
    return ret;
}
```

运行指令
```bash
./demoBin ../video/test.aac ../video/test_decode_by_code.pcm 

ffmpeg -f f32le -ar 44100 -ac 2 -i ../video/test_decode_by_code.pcm ../video/test_decode_by_code_stereo.wav

ffplay ../video/test_decode_by_code_stereo.wav 
```

# 音频编码
## 流程
| 函数名 | 描述 |
| --- | --- |
| `av_frame_alloc` | 分配一个AVFrame结构体 |
| `av_frame_get_buffer` | 为AVFrame分配缓冲区 |
| `avcodec_find_encoder_by_name` | 根据名称查找编码器 |
| `avcodec_alloc_context3` | 分配编码器上下文 |
| `avcodec_open2` | 打开编码器 |
| `avcodec_send_frame` | 发送帧到编码器 |
| `avcodec_receive_packet` | 从编码器接收编码后的数据包 |

运行指令
```bash
ffmpeg -ac 2 -ar 44100 -f s16le -i test.pcm -acodec libfdk_aac test1.aac

ffplay test1.aac
```

## 代码
[encodeAudioDemo.c](src/encodeAudioDemo.c)
```c
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
```
指令
```bash
./demoBin test.pcm  aac_by_code.aac

ffplay aac_by_code.aac
```

# 视频采集
## 视频采集命令
- 查看设备列表：
```bash
ffmpeg -hide_banner -devices
```
<img src="imagesForNotes/设备列表.png">

- 查看dshow支持的参数：
```bash
ffmpeg -h demuxer=dshow
```

- 查看dshow支持的设备：
```bash
ffmpeg -f dshow -list_devices true -i dummy
```
一般是`Integrated Camera`，这是本地摄像头

- 采集摄像头画面：
```bash
ffmpeg -f dshow -i video="Integrated Camera" ./video/output.mp4
```

播放摄像头采集画面：
```bash
ffplay output.mp4
```

## 流程
| 函数名 | 描述 |
| --- | --- |
| `avdevice_register_all` | 注册所有可用的设备 |
| `avformat_alloc_context` | 分配格式上下文 |
| `av_dict_set` | 设置字典选项 |
| `av_find_input_format` | 查找输入格式 |
| `avformat_open_input` | 打开输入文件 |
| `avformat_find_stream_info` | 查找流信息 |
| `av_find_best_stream` | 查找最佳流 |
| `avcodec_alloc_context3` | 分配编解码器上下文 |
| `avcode_parameters_to_context` | 将参数复制到上下文 |
| `avcodec_find_decoder` | 查找解码器 |
| `avcodec_open2` | 打开编解码器 |
| `av_read_frame` | 读取帧 |
| `avcode_send_packet` | 发送数据包 |
| `avcodec_receive_frame` | 接收帧 |

颜色空间格式转换：
| 函数名 | 描述 |
|--|--|
| `sws_getContext` | 获取缩放上下文 |
| `av_frame_alloc` | 分配帧 |
| `av_image_get_buffer_size` | 获取图像缓冲区大小 |
| `av_malloc` | 分配内存 |
| `av_image_fill_arrays` | 填充图像数组 |
| `sws_scale` | 缩放图像 |

先用ffmpeg指令试一下视频采集格式，后续代码写的时候要用对应采集的格式。

## 代码
```c
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

void decodeVideo(struct SwsContext *swsCtx, AVCodecContext *decoderCtx, AVFrame *destFrame,
                 AVPacket *packet, FILE *dest_fp)
{
    if (avcodec_send_packet(decoderCtx, packet) == 0)
    {
        AVFrame *frame = av_frame_alloc();
        while (avcodec_receive_frame(decoderCtx, frame) >= 0)
        {
            sws_scale(swsCtx, (const uint8_t *const *)frame->data, frame->linesize, 0,
                      decoderCtx->height, destFrame->data, destFrame->linesize);
            fwrite(destFrame->data[0], 1, decoderCtx->width * decoderCtx->height, dest_fp);
            fwrite(destFrame->data[1], 1, decoderCtx->width * decoderCtx->height / 4, dest_fp);
            fwrite(destFrame->data[2], 1, decoderCtx->width * decoderCtx->height / 4, dest_fp);
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
    av_dict_set(&options, "framerate", "30", 0);
    int ret = avformat_open_input(&inFmtCtx, "video=Integrated Camera", inFmt, &options);
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

    ret = av_find_best_stream(inFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "find best stream failed:%s\n", av_err2str(ret));
        goto end;
    }
    int videoIndex = ret;

    // 创建解码器上下文
    AVCodecContext *decoderCtx = avcodec_alloc_context3(NULL);

    ret = avcodec_parameters_to_context(decoderCtx, inFmtCtx->streams[videoIndex]->codecpar);
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
            if (packet->stream_index == videoIndex)
            {
                decodeVideo(swsCtx, decoderCtx, destFrame, packet, dest_fp);
            }
        }
        av_packet_unref(packet);
    }
    decodeVideo(swsCtx,decoderCtx, destFrame,NULL, dest_fp);

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
    if (outBuffer)
    {
        av_freep(&outBuffer);
    }
    return ret;
}
```

# 音频采集
## 音频采集命令

采集麦克风声音：
```bash
ffmpeg -f dshow -i audio="阵列麦克风 (AMD Audio Device)" -ar 44100 -f f32le output.pcm
```

播放麦克风采集：
```bash
ffplay -ar 44100 -f f32le output.pcm
```

## 流程
| 函数名 | 描述 |
| --- | --- |
| `avdevice_register_all` | 注册所有可用的设备 |
| `avformat_alloc_context` | 分配格式上下文 |
| `av_dict_set` | 设置字典选项 |
| `av_find_input_format` | 查找输入格式 |
| `avformat_open_input` | 打开输入文件 |
| `avformat_find_stream_info` | 查找流信息 |
| `av_find_best_stream` | 查找最佳流 |
| `avcodec_alloc_context3` | 分配编解码器上下文 |
| `avcode_parameters_to_context` | 将参数复制到上下文 |
| `avcodec_find_decoder` | 查找解码器 |
| `avcodec_open2` | 打开编解码器 |
| `av_read_frame` | 读取帧 |
| `avcode_send_packet` | 发送数据包 |
| `avcodec_receive_frame` | 接收帧 |

## 代码
```c
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
```
采集完后要用指令
```bash
ffplay -f s16le -ar 44100 code.pcm 
```
才可以播放，可能是参数的不同