#pragma once

#include <thread>
#include <atomic>
#include "ui/data.h"
#include "sysrc/MemoryUsage.h"

namespace data {
    static std::atomic_bool _data_thread_stop = false;

    void run_data_thread() {
        std::thread th([&]() {
            CMemoryUsage mem;
            if (!mem.Init()) {
                // todo: ֪ͨ
                exit(-1);
                // ...
            }

            data::_mem.max_bytes

            // !!!!data�ķ����Ƿ��̰߳�ȫ����������ν��͵����
            while (!_data_thread_stop)
            {
                mem.Update();

                // ˢ���ڴ�ռ��
                data::_mem.used_bytes.AddPoint(ImGui::GetTime(), mem.GetValue());
                mem.CopyProcessT(data::_mem.max_proc, sizeof(data::_mem.max_proc) / sizeof(data::_mem.max_proc[0]));

                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        });
        th.detach();
    }

    void stop_data_thread() {
        _data_thread_stop = true;
    }
};