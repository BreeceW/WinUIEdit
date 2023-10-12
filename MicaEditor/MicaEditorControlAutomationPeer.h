#pragma once

#include "MicaEditorControlAutomationPeer.g.h"

namespace winrt::MicaEditor::implementation
{
    struct MicaEditorControlAutomationPeer : MicaEditorControlAutomationPeerT<MicaEditorControlAutomationPeer>
    {
        MicaEditorControlAutomationPeer(MicaEditor::MicaEditorControl const &owner);

        hstring GetLocalizedControlTypeCore();
        Windows::Foundation::IInspectable GetPatternCore(DUX::Automation::Peers::PatternInterface const &patternInterface);
        hstring GetClassNameCore();
        DUX::Automation::Peers::AutomationControlType GetAutomationControlTypeCore();

        DUX::Automation::Provider::ITextRangeProvider DocumentRange();
        DUX::Automation::SupportedTextSelection SupportedTextSelection();
        com_array<DUX::Automation::Provider::ITextRangeProvider> GetSelection();
        com_array<DUX::Automation::Provider::ITextRangeProvider> GetVisibleRanges();
        DUX::Automation::Provider::ITextRangeProvider RangeFromChild(DUX::Automation::Provider::IRawElementProviderSimple const &childElement);
        DUX::Automation::Provider::ITextRangeProvider RangeFromPoint(Windows::Foundation::Point const &screenLocation);
        DUX::Automation::Provider::ITextRangeProvider GetActiveComposition();
        DUX::Automation::Provider::ITextRangeProvider GetConversionTarget();

        DUX::Automation::Provider::ITextRangeProvider RangeFromAnnotation(DUX::Automation::Provider::IRawElementProviderSimple const &annotationElement);
        DUX::Automation::Provider::ITextRangeProvider GetCaretRange(bool &isActive);

        bool IsReadOnly();
        hstring Value();
        void SetValue(hstring const &value);

    private:
        MicaEditor::Editor::UpdateUI_revoker _updateUIRevoker{};
        void Editor_UpdateUI(MicaEditor::Editor const &sender, MicaEditor::UpdateUIEventArgs const &args);
    };
}

namespace winrt::MicaEditor::factory_implementation
{
    struct MicaEditorControlAutomationPeer : MicaEditorControlAutomationPeerT<MicaEditorControlAutomationPeer, implementation::MicaEditorControlAutomationPeer>
    {
    };
}
