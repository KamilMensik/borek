// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

#include "Include/Core.h"
#include "Include/Debug/Assert.h"
#include "Include/Debug/WithChange.h"

namespace Borek {
namespace Graphics {

struct FrameBufferSettings {
        uint32_t width;
        uint32_t height;
};

class FrameBuffer {
        CHANGEABLE_OBJECT({
                OnChange();
        });
public:
        FrameBuffer(const FrameBufferSettings& s) : m_Settings(s) {}
        virtual ~FrameBuffer() {}
        static Ref<FrameBuffer> Create(const FrameBufferSettings& settings);
        inline const FrameBufferSettings& GetSettings() const
        {
                return m_Settings;
        }
        inline FrameBufferSettings& GetSettingsM()
        {
                BOREK_ENGINE_ASSERT(IS_CHANGING, "You need to use WITH_CHANGE macro");
                return m_Settings;
        }

        virtual void Bind() = 0;
        virtual void Unbind() = 0;
        virtual void OnChange() {}
        virtual uint32_t GetColorAttachmentId() = 0;

protected:
        FrameBufferSettings m_Settings;
        uint32_t m_Id = 0;
};

}  // namespace Graphics
}  // namespace Borek
