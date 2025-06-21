// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#ifndef _BOREK_RELEASE

#define CHANGEABLE_OBJECT(_block)                               \
private: bool m_Changing = false;                               \
public: inline void BeginChange() { m_Changing = true; }        \
public: inline void EndChange() { m_Changing = false; _block }

#define IS_CHANGING m_Changing
#define BEGIN_CHANGE IS_CHANGING = true;
#define STOP_CHANGE IS_CHANGING = false;

#define WITH_CHANGE(_obj, _block)        \
{                                        \
        (_obj).BeginChange();            \
        _block                           \
        (_obj).EndChange();              \
}

#else

#define WITH_CHANGE(_obj, _block) { _block, _obj.EndChange(); }

#define IS_CHANGING true
#define BEGIN_CHANGE;
#define STOP_CHANGE;

#define CHANGEABLE_OBJECT(_block)       \
public: inline void EndChange() _block

#endif
