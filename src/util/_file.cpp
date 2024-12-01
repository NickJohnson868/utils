#include "macro.h"
#include "_file.h"
#include "util.h"
#include "_string.h"
#include "_math.h"


#include <fstream>
#include <sstream>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>   // for flock
#include <errno.h>      // for errno and EWOULDBLOCK
#endif

#ifdef _WIN32
#ifndef __MINGW32__ // MINGW环境不适用
#ifdef _M_X64
#ifdef _DEBUG
#pragma comment(lib, "x64/OpenXLSXd.lib")
#else
#pragma comment(lib, "x64/OpenXLSX.lib")
#endif
#elif defined(_M_IX86)
#ifdef _DEBUG
#pragma comment(lib, "x86/OpenXLSXd.lib")
#else
#pragma comment(lib, "x86/OpenXLSX.lib")
#endif
#endif
#endif
#endif

void convert_progress(const std::string& command, long long total_duration);

std::string compress_command();

/* 内存中保持GBK编码 */
bool CFileUtil::read_file(const fs::path& path, std::string& data) {
    try {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            Util::color_print(P_ERROR, "Failed to open file: %s\n", path.c_str());
            return false;
        }
        data.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return true;
    }
    catch (const std::exception& e) {
        Util::color_print(P_ERROR, "Exception in read_file: %s\n", e.what());
        return false;
    }
}


/* 推荐写入UTF8 */
bool CFileUtil::write_file(const fs::path& path, const std::string& data, const bool& append) {
    try {
        auto flag = std::ios_base::binary;
        if (append)
            flag |= std::ios_base::app;
        std::ofstream file(path, flag);
        if (!file) {
            Util::color_print(P_ERROR, "Failed to open file for writing: %s\n", path.c_str());
            return false;
        }

        file << data;
        return true;
    }
    catch (const std::exception& e) {
        Util::color_print(P_ERROR, "Exception in write_file: %s\n", e.what());
        return false;
    }
}


bool CFileUtil::clear_file(const fs::path& path) {
    try {
        std::ofstream file(path, std::ios::trunc);
        return file.good();
    }
    catch (const std::exception& e) {
        Util::color_print(P_ERROR, "Exception in clear_file: %s\n", e.what());
        return false;
    }
}


bool CFileUtil::move_file(const fs::path& source_path, const fs::path& dest_path) {
    return rename_file(source_path, dest_path);
}


bool CFileUtil::rename_file(const fs::path& old_name, const fs::path& new_name) {
    try {
        if (fs::exists(new_name)) {
            Util::color_print(P_ERROR, "Destination file already exists: %s\n", new_name.string().c_str());
            return false;
        }
        fs::rename(old_name, new_name);
        return true;
    }
    catch (const fs::filesystem_error& e) {
        Util::color_print(P_ERROR, "Exception in rename_file: %s\n", e.what());
        return false;
    }
}


bool CFileUtil::delete_file(const fs::path& file_path) {
    try {
        fs::remove(file_path);
        return true;
    }
    catch (const fs::filesystem_error& e) {
        Util::color_print(P_ERROR, "Exception in delete_file: %s\n", e.what());
        return false;
    }
}

bool CFileUtil::is_hidden(const fs::path& file) {
#ifdef _WIN32
    DWORD fileAttributes = GetFileAttributesA(file.string().c_str());
    if (fileAttributes == INVALID_FILE_ATTRIBUTES)
    {
        Util::color_print(P_ERROR, "Failed to get attributes for file: %s\n", file.string().c_str());
        return false;
    }
    return fileAttributes & FILE_ATTRIBUTE_HIDDEN;
#elif __linux__
    std::string fileName = file.filename().string();
    return !fileName.empty() && fileName[0] == '.';
#endif
}

void CFileUtil::write_excel(OpenXLSX::XLWorksheet& wks, int row, int col, const std::string& data)
{
    try
    {
        wks.cell(OpenXLSX::XLCellReference(row, col)).value() = data;
    }
    catch (const std::exception& e)
    {
        Util::color_print(P_ERROR, "Exception in write_excel: %s\n", e.what());
    }
}


std::string CFileUtil::read_excel(OpenXLSX::XLWorksheet& wks, int row, int col)
{
    try
    {
        return wks.cell(OpenXLSX::XLCellReference(row, col)).value().get<std::string>();
    }
    catch (const std::exception& e)
    {
        Util::color_print(P_ERROR, "Exception in read_excel: %s\n", e.what());
        return "";
    }
}


void CFileUtil::ensure_directory_exists(const fs::path& dir) {
    try {
        if (!fs::exists(dir))
            fs::create_directories(dir);
    }
    catch (const fs::filesystem_error& e) {
        Util::color_print(P_ERROR, "Exception in ensure_directory_exists: %s\n", e.what());
    }
}


bool CFileUtil::is_exists(const fs::path& f) {
    return fs::exists(f);
}

bool CFileUtil::is_file(const fs::path& f) {
    return fs::is_regular_file(f);
}

bool CFileUtil::is_dir(const fs::path& f) {
    return fs::is_directory(f);
}

fs::path CFileUtil::get_exe_path() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
#elif __linux__
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len == -1) {
        Util::color_print(P_ERROR, "Failed to resolve executable path");
        len = 0;
    }
    buffer[len] = '\0'; // Null-terminate the buffer
#endif
    return std::filesystem::path(buffer);
}

fs::path CFileUtil::get_exe_dir() {
    return CFileUtil::get_exe_path().parent_path();
}


fs::path CFileUtil::get_parent_dir(const fs::path& file) {
    return file.parent_path();
}


fs::path CFileUtil::relative_path(const fs::path& p1, const fs::path& p2) {
    try {
        return p1.lexically_relative(p2);
    }
    catch (const std::exception& e) {
        Util::color_print(P_ERROR, "Exception in relative_path: %s\n", e.what());
        return fs::path();
    }
}


bool CFileUtil::is_file_in_use(const std::filesystem::path& filePath) {
#ifdef _WIN32
    HANDLE hFile = CreateFileW(
        filePath.wstring().data(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        if (error == ERROR_SHARING_VIOLATION) {
            return true; // 文件正在被使用
        }
        else {
            Util::color_print(P_ERROR, "Error: %lu\n", error);
        }
        return false;
    }

    CloseHandle(hFile); // 文件未被使用，可以正常关闭
    return false;
#elif __linux__
    int fd = open(filePath.c_str(), O_RDONLY); // 打开文件（只读）
    if (fd == -1) {
        perror("Failed to open file");
        return false;
    }

    // 尝试获得文件锁
    if (flock(fd, LOCK_EX | LOCK_NB) == -1) {
        if (errno == EWOULDBLOCK) {
            close(fd);
            return true; // 文件被占用
        }
        perror("Failed to lock file");
        close(fd);
        return false;
    }

    // 解锁并关闭文件
    flock(fd, LOCK_UN);
    close(fd);
    return false; // 文件未被占用
#endif
}


fs::path CFileUtil::change_extension(const fs::path& file, const std::string& new_extension) {
    /* 替换扩展名 */
    fs::path new_file = file;
    new_file.replace_extension(new_extension);
    return new_file;
}


bool CFileUtil::get_dir_files(const fs::path& dir, std::set<fs::path>& files, bool b_dir, bool b_recu) {
    auto process_entry = [&](const fs::directory_entry& entry) {
        bool is_file = fs::is_regular_file(entry);
        bool is_directory = fs::is_directory(entry);

        if ((b_dir && is_directory) || (!b_dir && is_file)) {
            files.insert(entry.path());
        }
        };

    try {
        if (!fs::exists(dir) || !fs::is_directory(dir)) {
            Util::color_print(P_ERROR, "Exception in get_dir_files: dir is not exists or dir is not a directory\n");
            return false;
        }
        if (b_recu)
            for (const auto& entry : fs::recursive_directory_iterator(dir))
                process_entry(entry);
        else
            for (const auto& entry : fs::directory_iterator(dir))
                process_entry(entry);
    }
    catch (const std::exception& e) {
        Util::color_print(P_ERROR, "Exception in get_dir_files: %s\n", e.what());
        return false;
    }

    return true;
}


bool CFileUtil::get_file_size(const fs::path& filePath, int& width, int& height) {
    nlohmann::json j;
    get_file_json_info(filePath, j);
    for (const auto& stream : j["streams"]) {
        if (stream["codec_type"] == "video") {
            width = stream["width"];
            height = stream["height"];
            return true;
        }
    }
    width = height = 0;
    return false;
}

bool CFileUtil::jpg_to_png(const fs::path& jpgPath, const fs::path& pngPath) {
    return png_to_jpg(jpgPath, pngPath);
}

bool CFileUtil::png_to_jpg(const fs::path& pngPath, const fs::path& jpgPath) {
    std::string command = ffmpeg + "ffmpeg -hide_banner -loglevel error -i \""
        + pngPath.string() + "\" \"" + jpgPath.string() + "\"";

    int result = std::system(command.c_str());
    if (result != 0) {
        return false;
    }
    return true;
}


std::string compress_command(const fs::path& filePath, int maxSize, double xScale, double yScale,
    int& width, int& height, const fs::path compress_path) {
    width *= xScale;
    height *= yScale;

    if (width > maxSize || height > maxSize) {
        double ratio = (std::max)(width, height) / static_cast<double>(maxSize);
        width = static_cast<int>(width / ratio);
        height = static_cast<int>(height / ratio);
    }

    /* 保持为偶数 */
    width = width / 2 * 2;
    height = height / 2 * 2;

    std::string inputFile = filePath.string();

    std::string command = ffmpeg + "ffmpeg -nostats -loglevel error -progress pipe:1 -hide_banner -y -i \"" + inputFile
        + "\" -vf \"scale=" + std::to_string(width) + ":" + std::to_string(height) +
        ",split[s0][s1];[s0]palettegen=reserve_transparent=1[p];[s1][p]paletteuse\" \"" +
        compress_path.string() + "\"";

    return command;
}

fs::path CFileUtil::compress_video(const fs::path& filePath, int maxSize, double xScale, double yScale, int& width,
    int& height) {
    std::string outputFile = filePath.parent_path().string() + FILE_SEP
        + filePath.stem().string()
        + "_compressed"
        + filePath.extension().string();
    std::string command = compress_command(filePath, maxSize, xScale, yScale, width, height, outputFile);
    long long duration = CVideoInfo(filePath).get_duration();
    convert_progress(command, duration);
    return outputFile;
}

fs::path CFileUtil::compress_image(const fs::path& filePath, int maxSize, double xScale, double yScale, int& width,
    int& height) {
    std::string outputFile = filePath.parent_path().string() + FILE_SEP
        + filePath.stem().string()
        + "_compressed"
        + filePath.extension().string();
    std::string command = compress_command(filePath, maxSize, xScale, yScale, width, height, outputFile);

    int result = system(command.data());
    if (result != 0) {
        Util::color_print(P_ERROR, "compress image error\n");
    }
    return outputFile;
}

bool CFileUtil::get_file_json_info(const fs::path& file, nlohmann::json& j) {
    if (!CFileUtil::is_exists(file))
        return false;
    std::string command = ffmpeg + "ffprobe -v quiet -print_format json -show_streams \"" + file.string() + "\"";
    std::string info_json;
    if (!Util::get_cmd_output(command, info_json)) {
        Util::color_print(P_ERROR, "get video info error\n");
        return false;
    }
    try {
        j = nlohmann::json::parse(info_json);
    }
    catch (const std::exception& e) {
        Util::color_print(P_ERROR, "get video info error: %s\n", e.what());
        return false;
    }
    return true;
}

void CFileUtil::ts_to_mp4(const fs::path& input_ts, const fs::path& output_mp4, std::string codec = "h264") {
    if (codec == "h264" || codec == "hevc") {
    }
    else if (codec == "h265") {
        codec = "hevc";
    }
    else {
        Util::color_print(P_ERROR, "unsupported video codec: %s\n", codec.data());
        return;
    }

    CVideoInfo video(input_ts);
    std::vector<std::string> codecs = video.get_codecs();
    std::string rule = "copy";
    auto it = std::find(codecs.begin(), codecs.end(), codec);
    if (it == codecs.end()) {
        rule = codec; /* 不存在相同的编码，使用转换 */
    }

    std::string command = ffmpeg + "ffmpeg -y -hide_banner -loglevel error -i \""
        + input_ts.string()
        + "\" -c:v " + codec +
        "_nvenc -x265-params log-level=0 -c:a copy -progress pipe:1 -nostats \""
        + output_mp4.string() + "\"";

    std::cout << "Converting " << input_ts << std::endl;
    long long total_duration = video.get_duration();
    if (total_duration != 0) {
        convert_progress(command, total_duration);
    }
    else {
        if (system(command.c_str()) != 0)
            Util::color_print(P_ERROR, "error during video conversion\n");
    }
}


void CFileUtil::mp4_to_ts(const fs::path& input_mp4, const fs::path& output_ts) {
    /* 获取视频编码信息 */
    nlohmann::json j;
    if (!get_file_json_info(input_mp4, j)) {
        Util::color_print(P_ERROR, "Error getting video info.\n");
        return;
    }
    bool need_transcode = false;
    double total_duration = 0; /* 毫秒 */

    for (const auto& stream : j["streams"]) {
        if (stream["codec_type"] == "video") {
            std::string codec_name = stream["codec_name"];
            total_duration = std::stoll(std::string(stream["duration"])) * 1000;
            if (codec_name != "h264") {
                need_transcode = true;
                break;
            }
        }
    }

    if (total_duration == 0)
        return;

    std::cout << "Converting " << input_mp4 << std::endl;
    std::string command;
    if (need_transcode) {
        /* 先转换为 H.264 编码，再转换为 TS 格式 */
        command = ffmpeg + "ffmpeg -y -hide_banner -loglevel error -i \"" + input_mp4.string() +
            "\" -c:v h264_nvenc -c:a copy -f mpegts -progress pipe:1 \"" + output_ts.string() + "\"";
    }
    else {
        /* 直接复制编码到 TS 格式 */
        command = ffmpeg + "ffmpeg -y -hide_banner -loglevel error -i \"" + input_mp4.string() +
            "\" -c:v copy -c:a copy -f mpegts -progress pipe:1 \"" + output_ts.string() + "\"";
    }

    convert_progress(command, total_duration);
}


void convert_progress(const std::string& command, long long total_duration) {
    FILE* pipe = _popen(command.c_str(), OPEN_MODE);
    if (!pipe) {
        Util::color_print(P_ERROR, "Error opening pipe for ffmpeg.\n");
        return;
    }

    char line[256];
    bool cursor_visible = Util::is_cursor_visible();
    Util::set_cursor_visible(false);
    Util::print_progress_bar(0, total_duration);
    while (fgets(line, sizeof(line), pipe) != nullptr) {
        std::string str = CStringUtil::trim(line);
        if (str.find("out_time_us") != std::string::npos) {
            std::string time = str.substr(str.find('=') + 1);
            double out_time_us = 0;
            if (CMathUtil::is_numeric(time)) {
                out_time_us = std::stod(time.data());
                Util::print_progress_bar(out_time_us / 1000, total_duration);
            }
        }
        if (str.find("progress") != std::string::npos) {
            std::string progress = str.substr(str.find('=') + 1);
            if (progress == "end") {
                Util::print_progress_bar(total_duration, total_duration);
                break;
            }
        }
    }
    printf("\n");
    Util::set_cursor_visible(cursor_visible);
    _pclose(pipe);
}


/*
 * extern "C" {
 * #include <libavformat/avformat.h>
 * #include <libavcodec/avcodec.h>
 * #include <libavutil/opt.h>
 * #include <libavutil/imgutils.h>
 * #include <libswscale/swscale.h>
 * }
 *
 * #pragma comment(lib, "avformat.lib")
 * #pragma comment(lib, "avfilter.lib")
 * #pragma comment(lib, "avutil.lib")
 * #pragma comment(lib, "avcodec.lib")
 * #pragma comment(lib, "swresample.lib")
 * #pragma comment(lib, "winmm.lib")
 *
 * // ffmpeg 4
 * int convert_ts_to_mp4(const char* input_ts, const char* output_mp4, std::string codec = "h264")
 * {
 * AVCodecID id;
 * if (codec == "h264") id = AV_CODEC_ID_H264;
 * else if (codec == "h265") id = AV_CODEC_ID_H265;
 * else return -1;
 *
 * AVFormatContext* input_ctx = nullptr;
 * AVFormatContext* output_ctx = nullptr;
 * AVCodecContext* decoder_ctx = nullptr;
 * AVCodecContext* encoder_ctx = nullptr;
 * AVCodecContext* audio_decoder_ctx = nullptr;
 * AVCodecContext* audio_encoder_ctx = nullptr;
 * AVStream* video_stream = nullptr;
 * AVStream* audio_stream = nullptr;
 * AVStream* out_video_stream = nullptr;
 * AVStream* out_audio_stream = nullptr;
 *
 * int ret = 0;
 *
 * // 打开输入文件
 * if ((ret = avformat_open_input(&input_ctx, input_ts, nullptr, nullptr)) < 0) {
 *      av_log(nullptr, AV_LOG_ERROR, "Failed to open input file.\n");
 *      return ret;
 * }
 *
 * // 查找流信息
 * if ((ret = avformat_find_stream_info(input_ctx, nullptr)) < 0) {
 *      av_log(nullptr, AV_LOG_ERROR, "Failed to find stream info.\n");
 *      return ret;
 * }
 *
 * // 打开输出文件
 * avformat_alloc_output_context2(&output_ctx, nullptr, "mp4", output_mp4);
 * if (!output_ctx) {
 *      av_log(nullptr, AV_LOG_ERROR, "Could not allocate output context.\n");
 *      return AVERROR_UNKNOWN;
 * }
 *
 * // 处理视频流
 * AVCodec* decoder = avcodec_find_decoder(id);
 * if (!decoder) {
 *      av_log(nullptr, AV_LOG_ERROR, "H.264 decoder not found.\n");
 *      return AVERROR_DECODER_NOT_FOUND;
 * }
 *
 * int video_stream_index = av_find_best_stream(input_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
 * if (video_stream_index < 0) {
 *      av_log(nullptr, AV_LOG_ERROR, "Failed to find video stream in input file.\n");
 *      return AVERROR_STREAM_NOT_FOUND;
 * }
 *
 * decoder_ctx = avcodec_alloc_context3(decoder);
 * decoder_ctx->thread_count = 4; // 设置多线程
 * avcodec_parameters_to_context(decoder_ctx, input_ctx->streams[video_stream_index]->codecpar);
 * avcodec_open2(decoder_ctx, decoder, nullptr);
 *
 * encoder_ctx = avcodec_alloc_context3(avcodec_find_encoder(id));
 * encoder_ctx->width = decoder_ctx->width;
 * encoder_ctx->height = decoder_ctx->height;
 * encoder_ctx->pix_fmt = encoder_ctx->codec->pix_fmts[0];
 * encoder_ctx->time_base = input_ctx->streams[video_stream_index]->time_base;
 * encoder_ctx->thread_count = 4; // 设置多线程
 * avcodec_open2(encoder_ctx, avcodec_find_encoder(id), nullptr);
 *
 * out_video_stream = avformat_new_stream(output_ctx, nullptr);
 * avcodec_parameters_from_context(out_video_stream->codecpar, encoder_ctx);
 * out_video_stream->time_base = encoder_ctx->time_base;
 *
 * // 处理音频流
 * AVCodec* audio_decoder = avcodec_find_decoder(AV_CODEC_ID_AAC); // 假设音频编码为AAC
 * if (!audio_decoder) {
 *      av_log(nullptr, AV_LOG_ERROR, "AAC decoder not found.\n");
 *      return AVERROR_DECODER_NOT_FOUND;
 * }
 *
 * int audio_stream_index = av_find_best_stream(input_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &audio_decoder, 0);
 * if (audio_stream_index < 0) {
 *      av_log(nullptr, AV_LOG_ERROR, "Failed to find audio stream in input file.\n");
 *      return AVERROR_STREAM_NOT_FOUND;
 * }
 *
 * audio_decoder_ctx = avcodec_alloc_context3(audio_decoder);
 * avcodec_parameters_to_context(audio_decoder_ctx, input_ctx->streams[audio_stream_index]->codecpar);
 * avcodec_open2(audio_decoder_ctx, audio_decoder, nullptr);
 *
 * AVCodec* audio_encoder = avcodec_find_encoder(AV_CODEC_ID_AAC);
 * if (!audio_encoder) {
 *      av_log(nullptr, AV_LOG_ERROR, "AAC encoder not found.\n");
 *      return AVERROR_ENCODER_NOT_FOUND;
 * }
 *
 * audio_encoder_ctx = avcodec_alloc_context3(audio_encoder);
 * audio_encoder_ctx->channels = audio_decoder_ctx->channels;
 * audio_encoder_ctx->channel_layout = av_get_default_channel_layout(audio_encoder_ctx->channels);
 * audio_encoder_ctx->sample_rate = audio_decoder_ctx->sample_rate;
 * audio_encoder_ctx->sample_fmt = audio_encoder->sample_fmts[0];
 * audio_encoder_ctx->time_base = input_ctx->streams[audio_stream_index]->time_base;
 * avcodec_open2(audio_encoder_ctx, audio_encoder, nullptr);
 *
 * out_audio_stream = avformat_new_stream(output_ctx, nullptr);
 * avcodec_parameters_from_context(out_audio_stream->codecpar, audio_encoder_ctx);
 * out_audio_stream->time_base = audio_encoder_ctx->time_base;
 *
 * // 打开输出文件
 * if (!(output_ctx->oformat->flags & AVFMT_NOFILE)) {
 *      ret = avio_open(&output_ctx->pb, output_mp4, AVIO_FLAG_WRITE);
 *      if (ret < 0) {
 *              av_log(nullptr, AV_LOG_ERROR, "Could not open output file.\n");
 *              return ret;
 *      }
 * }
 *
 * ret = avformat_write_header(output_ctx, nullptr);
 * if (ret < 0) {
 *      av_log(nullptr, AV_LOG_ERROR, "Error occurred when opening output file.\n");
 *      return ret;
 * }
 *
 * // 数据包处理循环
 * AVPacket* packet = av_packet_alloc();
 * AVFrame* frame = av_frame_alloc();
 *
 * while (av_read_frame(input_ctx, packet) >= 0) {
 *      if (packet->stream_index == video_stream_index) {
 *              // 处理视频流
 *              ret = avcodec_send_packet(decoder_ctx, packet);
 *              if (ret < 0) continue;
 *
 *              while (ret >= 0) {
 *                      ret = avcodec_receive_frame(decoder_ctx, frame);
 *                      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
 *                      else if (ret < 0) continue;
 *
 *                      ret = avcodec_send_frame(encoder_ctx, frame);
 *                      if (ret < 0) continue;
 *
 *                      while (ret >= 0) {
 *                              ret = avcodec_receive_packet(encoder_ctx, packet);
 *                              if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
 *                              else if (ret < 0) continue;
 *
 *                              av_packet_rescale_ts(packet, encoder_ctx->time_base, out_video_stream->time_base);
 *                              packet->stream_index = out_video_stream->index;
 *                              av_interleaved_write_frame(output_ctx, packet);
 *                      }
 *              }
 *      }
 *      else if (packet->stream_index == audio_stream_index) {
 *              // 处理音频流
 *              ret = avcodec_send_packet(audio_decoder_ctx, packet);
 *              if (ret < 0) continue;
 *
 *              while (ret >= 0) {
 *                      ret = avcodec_receive_frame(audio_decoder_ctx, frame);
 *                      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
 *                      else if (ret < 0) continue;
 *
 *                      ret = avcodec_send_frame(audio_encoder_ctx, frame);
 *                      if (ret < 0) continue;
 *
 *                      while (ret >= 0) {
 *                              ret = avcodec_receive_packet(audio_encoder_ctx, packet);
 *                              if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
 *                              else if (ret < 0) continue;
 *
 *                              av_packet_rescale_ts(packet, audio_encoder_ctx->time_base, out_audio_stream->time_base);
 *                              packet->stream_index = out_audio_stream->index;
 *                              av_interleaved_write_frame(output_ctx, packet);
 *                      }
 *              }
 *      }
 *      av_packet_unref(packet);
 * }
 *
 * // 刷新编码器缓冲区
 * avcodec_send_frame(encoder_ctx, nullptr);
 * avcodec_send_frame(audio_encoder_ctx, nullptr);
 *
 * while (avcodec_receive_packet(encoder_ctx, packet) >= 0) {
 *      av_packet_rescale_ts(packet, encoder_ctx->time_base, out_video_stream->time_base);
 *      packet->stream_index = out_video_stream->index;
 *      av_interleaved_write_frame(output_ctx, packet);
 * }
 *
 * while (avcodec_receive_packet(audio_encoder_ctx, packet) >= 0) {
 *      av_packet_rescale_ts(packet, audio_encoder_ctx->time_base, out_audio_stream->time_base);
 *      packet->stream_index = out_audio_stream->index;
 *      av_interleaved_write_frame(output_ctx, packet);
 * }
 *
 * av_write_trailer(output_ctx);
 *
 * // 释放资源
 * av_frame_free(&frame);
 * av_packet_free(&packet);
 * avcodec_free_context(&decoder_ctx);
 * avcodec_free_context(&encoder_ctx);
 * avcodec_free_context(&audio_decoder_ctx);
 * avcodec_free_context(&audio_encoder_ctx);
 * avformat_close_input(&input_ctx);
 * avformat_free_context(output_ctx);
 *
 * return 0;
 * }
 */


CVideoInfo::CVideoInfo(fs::path filepath)
    : m_filepath(filepath)
    , m_fps(0)
    , m_width(0)
    , m_height(0)
    , m_codecs({})
    , m_duration(0) {
    init();
}

void CVideoInfo::init() {
    if (CFileUtil::is_exists(m_filepath) && CFileUtil::is_file(m_filepath)) {
        nlohmann::json j;
        if (!CFileUtil::get_file_json_info(m_filepath, j))
            return;

        for (const auto& stream : j["streams"]) {
            if (stream["codec_type"] == "video") {
                std::string codec_name = stream["codec_name"];
                m_codecs.push_back(codec_name);

                m_width = stream["width"];
                m_height = stream["height"];

                if (stream.contains("avg_frame_rate")) {
                    std::string avg_frame_rate = stream["avg_frame_rate"];
                    int num, den;
                    sscanf_s(avg_frame_rate.c_str(), "%d/%d", &num, &den);
                    m_fps = static_cast<double>(num) / den;
                }

                if (stream.contains("nb_frames")) {
                    std::string frames = stream["nb_frames"];
                    m_maxFrame = std::atoi(frames.data());
                }

                if (stream.contains("duration")) {
                    std::string dur = stream["duration"];
                    m_duration = std::stoll(dur.data()) * 1000;
                }
            }
        }
    }
}