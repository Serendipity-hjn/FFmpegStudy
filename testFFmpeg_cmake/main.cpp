#include <iostream>
using namespace std;

extern "C" {
#include "libavcodec/avcodec.h"
}
/// 由于我们建立的是C++的工程
/// 编译的时候使用的C++的编译器编译
/// 而FFMPEG是C的库
/// 因此这里需要加上extern "C"
/// 否则会提示各种未定义

int main() {
  // 这里简单的输出一个版本号
  cout << "Hello FFmpeg!" << endl;
  unsigned version = avcodec_version();
  cout << "version is:" << version << endl;

  return 0;
}