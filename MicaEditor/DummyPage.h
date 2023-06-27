#pragma once

#include "DummyPage.g.h"

#include "ControlIncludes.h"

namespace winrt::MicaEditor::implementation
{
    struct DummyPage : DummyPageT<DummyPage>
    {
    };
}

namespace winrt::MicaEditor::factory_implementation
{
    struct DummyPage : DummyPageT<DummyPage, implementation::DummyPage>
    {
    };
}
