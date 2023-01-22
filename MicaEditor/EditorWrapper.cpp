#include "pch.h"
#include "EditorWrapper.h"
#include "Editor.g.cpp"

namespace winrt::MicaEditor::implementation
{
	StyleNeededEventArgs::StyleNeededEventArgs(int32_t position)
		: _position { position }
	{
	}

	int32_t StyleNeededEventArgs::Position()
	{
		return _position;
	}

	CharAddedEventArgs::CharAddedEventArgs(int32_t ch, int32_t characterSource)
		: _ch { ch }, _characterSource { characterSource }
	{
	}

	int32_t CharAddedEventArgs::Ch()
	{
		return _ch;
	}

	int32_t CharAddedEventArgs::CharacterSource()
	{
		return _characterSource;
	}

	KeyEventArgs::KeyEventArgs(int32_t ch, int32_t modifiers)
		: _ch { ch }, _modifiers { modifiers }
	{
	}

	int32_t KeyEventArgs::Ch()
	{
		return _ch;
	}

	int32_t KeyEventArgs::Modifiers()
	{
		return _modifiers;
	}

	DoubleClickEventArgs::DoubleClickEventArgs(int32_t modifiers, int32_t position, int32_t line)
		: _modifiers { modifiers }, _position { position }, _line { line }
	{
	}

	int32_t DoubleClickEventArgs::Modifiers()
	{
		return _modifiers;
	}

	int32_t DoubleClickEventArgs::Position()
	{
		return _position;
	}

	int32_t DoubleClickEventArgs::Line()
	{
		return _line;
	}

	UpdateUIEventArgs::UpdateUIEventArgs(int32_t updated)
		: _updated { updated }
	{
	}

	int32_t UpdateUIEventArgs::Updated()
	{
		return _updated;
	}

	ModifiedEventArgs::ModifiedEventArgs(int32_t position, int32_t modificationType, const char *text, int32_t length, int32_t linesAdded, int32_t line, int32_t foldLevelNow, int32_t foldLevelPrev, int32_t token, int32_t annotationLinesAdded)
		: _position { position }, _modificationType { modificationType }, _textAsPointer { text }, _length { length }, _linesAdded { linesAdded }, _line { line }, _foldLevelNow { foldLevelNow }, _foldLevelPrev { foldLevelPrev }, _token { token }, _annotationLinesAdded { annotationLinesAdded }
	{
	}

	int32_t ModifiedEventArgs::Position()
	{
		return _position;
	}

	int32_t ModifiedEventArgs::ModificationType()
	{
		return _modificationType;
	}

	Windows::Storage::Streams::IBuffer ModifiedEventArgs::TextAsBuffer()
	{
		if (!_textAsBuffer)
		{
			if (_textAsPointer)
			{
				_textAsBuffer = Windows::Storage::Streams::Buffer{ static_cast<uint32_t>(_length) };
				memcpy(_textAsBuffer.data(), _textAsPointer, _length);
				_textAsBuffer.Length(_length);
			}
			else
			{
				_textAsBuffer = Windows::Storage::Streams::Buffer{ 0u };
			}
		}
		return _textAsBuffer;
	}

	hstring ModifiedEventArgs::Text()
	{
		if (!_text)
		{
			_text = _textAsPointer ? to_hstring(std::string_view{ _textAsPointer, static_cast<size_t>(_length) }) : L"";
		}
		return _text.GetString();
	}

	int32_t ModifiedEventArgs::Length()
	{
		return _length;
	}

	int32_t ModifiedEventArgs::LinesAdded()
	{
		return _linesAdded;
	}

	int32_t ModifiedEventArgs::Line()
	{
		return _line;
	}

	int32_t ModifiedEventArgs::FoldLevelNow()
	{
		return _foldLevelNow;
	}

	int32_t ModifiedEventArgs::FoldLevelPrev()
	{
		return _foldLevelPrev;
	}

	int32_t ModifiedEventArgs::Token()
	{
		return _token;
	}

	int32_t ModifiedEventArgs::AnnotationLinesAdded()
	{
		return _annotationLinesAdded;
	}

	MacroRecordEventArgs::MacroRecordEventArgs(int32_t message, int32_t wParam, int32_t lParam)
		: _message { message }, _wParam { wParam }, _lParam { lParam }
	{
	}

	int32_t MacroRecordEventArgs::Message()
	{
		return _message;
	}

	int32_t MacroRecordEventArgs::WParam()
	{
		return _wParam;
	}

	int32_t MacroRecordEventArgs::LParam()
	{
		return _lParam;
	}

	MarginClickEventArgs::MarginClickEventArgs(int32_t modifiers, int32_t position, int32_t margin)
		: _modifiers { modifiers }, _position { position }, _margin { margin }
	{
	}

	int32_t MarginClickEventArgs::Modifiers()
	{
		return _modifiers;
	}

	int32_t MarginClickEventArgs::Position()
	{
		return _position;
	}

	int32_t MarginClickEventArgs::Margin()
	{
		return _margin;
	}

	NeedShownEventArgs::NeedShownEventArgs(int32_t position, int32_t length)
		: _position { position }, _length { length }
	{
	}

	int32_t NeedShownEventArgs::Position()
	{
		return _position;
	}

	int32_t NeedShownEventArgs::Length()
	{
		return _length;
	}

	UserListSelectionEventArgs::UserListSelectionEventArgs(int32_t listType, const char *text, int32_t position, int32_t ch, MicaEditor::CompletionMethods const &listCompletionMethod)
		: _listType { listType }, _textAsPointer { text }, _position { position }, _ch { ch }, _listCompletionMethod { listCompletionMethod }
	{
	}

	int32_t UserListSelectionEventArgs::ListType()
	{
		return _listType;
	}

	Windows::Storage::Streams::IBuffer UserListSelectionEventArgs::TextAsBuffer()
	{
		if (!_textAsBuffer)
		{
			if (_textAsPointer)
			{
				const auto length { strnlen_s(_textAsPointer, std::numeric_limits<uint32_t>::max()) };
				_textAsBuffer = Windows::Storage::Streams::Buffer{ static_cast<uint32_t>(length) };
				memcpy(_textAsBuffer.data(), _textAsPointer, length);
				_textAsBuffer.Length(length);
			}
			else
			{
				_textAsBuffer = Windows::Storage::Streams::Buffer{ 0u };
			}
		}
		return _textAsBuffer;
	}

	hstring UserListSelectionEventArgs::Text()
	{
		if (!_text)
		{
			_text = _textAsPointer ? to_hstring(_textAsPointer) : L"";
		}
		return _text.GetString();
	}

	int32_t UserListSelectionEventArgs::Position()
	{
		return _position;
	}

	int32_t UserListSelectionEventArgs::Ch()
	{
		return _ch;
	}

	MicaEditor::CompletionMethods UserListSelectionEventArgs::ListCompletionMethod()
	{
		return _listCompletionMethod;
	}

	URIDroppedEventArgs::URIDroppedEventArgs(const char *text)
		: _textAsPointer { text }
	{
	}

	Windows::Storage::Streams::IBuffer URIDroppedEventArgs::TextAsBuffer()
	{
		if (!_textAsBuffer)
		{
			if (_textAsPointer)
			{
				const auto length { strnlen_s(_textAsPointer, std::numeric_limits<uint32_t>::max()) };
				_textAsBuffer = Windows::Storage::Streams::Buffer{ static_cast<uint32_t>(length) };
				memcpy(_textAsBuffer.data(), _textAsPointer, length);
				_textAsBuffer.Length(length);
			}
			else
			{
				_textAsBuffer = Windows::Storage::Streams::Buffer{ 0u };
			}
		}
		return _textAsBuffer;
	}

	hstring URIDroppedEventArgs::Text()
	{
		if (!_text)
		{
			_text = _textAsPointer ? to_hstring(_textAsPointer) : L"";
		}
		return _text.GetString();
	}

	DwellStartEventArgs::DwellStartEventArgs(int32_t position, int32_t x, int32_t y)
		: _position { position }, _x { x }, _y { y }
	{
	}

	int32_t DwellStartEventArgs::Position()
	{
		return _position;
	}

	int32_t DwellStartEventArgs::X()
	{
		return _x;
	}

	int32_t DwellStartEventArgs::Y()
	{
		return _y;
	}

	DwellEndEventArgs::DwellEndEventArgs(int32_t position, int32_t x, int32_t y)
		: _position { position }, _x { x }, _y { y }
	{
	}

	int32_t DwellEndEventArgs::Position()
	{
		return _position;
	}

	int32_t DwellEndEventArgs::X()
	{
		return _x;
	}

	int32_t DwellEndEventArgs::Y()
	{
		return _y;
	}

	HotSpotClickEventArgs::HotSpotClickEventArgs(int32_t modifiers, int32_t position)
		: _modifiers { modifiers }, _position { position }
	{
	}

	int32_t HotSpotClickEventArgs::Modifiers()
	{
		return _modifiers;
	}

	int32_t HotSpotClickEventArgs::Position()
	{
		return _position;
	}

	HotSpotDoubleClickEventArgs::HotSpotDoubleClickEventArgs(int32_t modifiers, int32_t position)
		: _modifiers { modifiers }, _position { position }
	{
	}

	int32_t HotSpotDoubleClickEventArgs::Modifiers()
	{
		return _modifiers;
	}

	int32_t HotSpotDoubleClickEventArgs::Position()
	{
		return _position;
	}

	CallTipClickEventArgs::CallTipClickEventArgs(int32_t position)
		: _position { position }
	{
	}

	int32_t CallTipClickEventArgs::Position()
	{
		return _position;
	}

	AutoCSelectionEventArgs::AutoCSelectionEventArgs(const char *text, int32_t position, int32_t ch, MicaEditor::CompletionMethods const &listCompletionMethod)
		: _textAsPointer { text }, _position { position }, _ch { ch }, _listCompletionMethod { listCompletionMethod }
	{
	}

	Windows::Storage::Streams::IBuffer AutoCSelectionEventArgs::TextAsBuffer()
	{
		if (!_textAsBuffer)
		{
			if (_textAsPointer)
			{
				const auto length { strnlen_s(_textAsPointer, std::numeric_limits<uint32_t>::max()) };
				_textAsBuffer = Windows::Storage::Streams::Buffer{ static_cast<uint32_t>(length) };
				memcpy(_textAsBuffer.data(), _textAsPointer, length);
				_textAsBuffer.Length(length);
			}
			else
			{
				_textAsBuffer = Windows::Storage::Streams::Buffer{ 0u };
			}
		}
		return _textAsBuffer;
	}

	hstring AutoCSelectionEventArgs::Text()
	{
		if (!_text)
		{
			_text = _textAsPointer ? to_hstring(_textAsPointer) : L"";
		}
		return _text.GetString();
	}

	int32_t AutoCSelectionEventArgs::Position()
	{
		return _position;
	}

	int32_t AutoCSelectionEventArgs::Ch()
	{
		return _ch;
	}

	MicaEditor::CompletionMethods AutoCSelectionEventArgs::ListCompletionMethod()
	{
		return _listCompletionMethod;
	}

	IndicatorClickEventArgs::IndicatorClickEventArgs(int32_t modifiers, int32_t position)
		: _modifiers { modifiers }, _position { position }
	{
	}

	int32_t IndicatorClickEventArgs::Modifiers()
	{
		return _modifiers;
	}

	int32_t IndicatorClickEventArgs::Position()
	{
		return _position;
	}

	IndicatorReleaseEventArgs::IndicatorReleaseEventArgs(int32_t modifiers, int32_t position)
		: _modifiers { modifiers }, _position { position }
	{
	}

	int32_t IndicatorReleaseEventArgs::Modifiers()
	{
		return _modifiers;
	}

	int32_t IndicatorReleaseEventArgs::Position()
	{
		return _position;
	}

	HotSpotReleaseClickEventArgs::HotSpotReleaseClickEventArgs(int32_t modifiers, int32_t position)
		: _modifiers { modifiers }, _position { position }
	{
	}

	int32_t HotSpotReleaseClickEventArgs::Modifiers()
	{
		return _modifiers;
	}

	int32_t HotSpotReleaseClickEventArgs::Position()
	{
		return _position;
	}

	AutoCCompletedEventArgs::AutoCCompletedEventArgs(const char *text, int32_t position, int32_t ch, MicaEditor::CompletionMethods const &listCompletionMethod)
		: _textAsPointer { text }, _position { position }, _ch { ch }, _listCompletionMethod { listCompletionMethod }
	{
	}

	Windows::Storage::Streams::IBuffer AutoCCompletedEventArgs::TextAsBuffer()
	{
		if (!_textAsBuffer)
		{
			if (_textAsPointer)
			{
				const auto length { strnlen_s(_textAsPointer, std::numeric_limits<uint32_t>::max()) };
				_textAsBuffer = Windows::Storage::Streams::Buffer{ static_cast<uint32_t>(length) };
				memcpy(_textAsBuffer.data(), _textAsPointer, length);
				_textAsBuffer.Length(length);
			}
			else
			{
				_textAsBuffer = Windows::Storage::Streams::Buffer{ 0u };
			}
		}
		return _textAsBuffer;
	}

	hstring AutoCCompletedEventArgs::Text()
	{
		if (!_text)
		{
			_text = _textAsPointer ? to_hstring(_textAsPointer) : L"";
		}
		return _text.GetString();
	}

	int32_t AutoCCompletedEventArgs::Position()
	{
		return _position;
	}

	int32_t AutoCCompletedEventArgs::Ch()
	{
		return _ch;
	}

	MicaEditor::CompletionMethods AutoCCompletedEventArgs::ListCompletionMethod()
	{
		return _listCompletionMethod;
	}

	MarginRightClickEventArgs::MarginRightClickEventArgs(int32_t modifiers, int32_t position, int32_t margin)
		: _modifiers { modifiers }, _position { position }, _margin { margin }
	{
	}

	int32_t MarginRightClickEventArgs::Modifiers()
	{
		return _modifiers;
	}

	int32_t MarginRightClickEventArgs::Position()
	{
		return _position;
	}

	int32_t MarginRightClickEventArgs::Margin()
	{
		return _margin;
	}

	AutoCSelectionChangeEventArgs::AutoCSelectionChangeEventArgs(int32_t listType, const char *text, int32_t position)
		: _listType { listType }, _textAsPointer { text }, _position { position }
	{
	}

	int32_t AutoCSelectionChangeEventArgs::ListType()
	{
		return _listType;
	}

	Windows::Storage::Streams::IBuffer AutoCSelectionChangeEventArgs::TextAsBuffer()
	{
		if (!_textAsBuffer)
		{
			if (_textAsPointer)
			{
				const auto length { strnlen_s(_textAsPointer, std::numeric_limits<uint32_t>::max()) };
				_textAsBuffer = Windows::Storage::Streams::Buffer{ static_cast<uint32_t>(length) };
				memcpy(_textAsBuffer.data(), _textAsPointer, length);
				_textAsBuffer.Length(length);
			}
			else
			{
				_textAsBuffer = Windows::Storage::Streams::Buffer{ 0u };
			}
		}
		return _textAsBuffer;
	}

	hstring AutoCSelectionChangeEventArgs::Text()
	{
		if (!_text)
		{
			_text = _textAsPointer ? to_hstring(_textAsPointer) : L"";
		}
		return _text.GetString();
	}

	int32_t AutoCSelectionChangeEventArgs::Position()
	{
		return _position;
	}

	Editor::Editor(com_ptr<Scintilla::Internal::ScintillaWinUI> const &scintilla)
		: _scintilla{ scintilla }
	{
	}

	void Editor::ProcessEvent(Scintilla::NotificationData *data)
	{
		switch (static_cast<int>(data->nmhdr.code))
		{
		case 2000: // StyleNeeded
		{
			if (_styleNeededEvent)
			{
				_styleNeededEvent(*this, *make_self<implementation::StyleNeededEventArgs>(static_cast<int32_t>(data->position)));
			}
		}
		break;
		case 2001: // CharAdded
		{
			if (_charAddedEvent)
			{
				_charAddedEvent(*this, *make_self<implementation::CharAddedEventArgs>(static_cast<int32_t>(data->ch), static_cast<int32_t>(data->characterSource)));
			}
		}
		break;
		case 2002: // SavePointReached
		{
			if (_savePointReachedEvent)
			{
				_savePointReachedEvent(*this, *make_self<implementation::SavePointReachedEventArgs>());
			}
		}
		break;
		case 2003: // SavePointLeft
		{
			if (_savePointLeftEvent)
			{
				_savePointLeftEvent(*this, *make_self<implementation::SavePointLeftEventArgs>());
			}
		}
		break;
		case 2004: // ModifyAttemptRO
		{
			if (_modifyAttemptROEvent)
			{
				_modifyAttemptROEvent(*this, *make_self<implementation::ModifyAttemptROEventArgs>());
			}
		}
		break;
		case 2005: // Key
		{
			if (_keyEvent)
			{
				_keyEvent(*this, *make_self<implementation::KeyEventArgs>(static_cast<int32_t>(data->ch), static_cast<int32_t>(data->modifiers)));
			}
		}
		break;
		case 2006: // DoubleClick
		{
			if (_doubleClickEvent)
			{
				_doubleClickEvent(*this, *make_self<implementation::DoubleClickEventArgs>(static_cast<int32_t>(data->modifiers), static_cast<int32_t>(data->position), static_cast<int32_t>(data->line)));
			}
		}
		break;
		case 2007: // UpdateUI
		{
			if (_updateUIEvent)
			{
				_updateUIEvent(*this, *make_self<implementation::UpdateUIEventArgs>(static_cast<int32_t>(data->updated)));
			}
		}
		break;
		case 2008: // Modified
		{
			if (_modifiedEvent)
			{
				_modifiedEvent(*this, *make_self<implementation::ModifiedEventArgs>(static_cast<int32_t>(data->position), static_cast<int32_t>(data->modificationType), data->text, static_cast<int32_t>(data->length), static_cast<int32_t>(data->linesAdded), static_cast<int32_t>(data->line), static_cast<int32_t>(data->foldLevelNow), static_cast<int32_t>(data->foldLevelPrev), static_cast<int32_t>(data->token), static_cast<int32_t>(data->annotationLinesAdded)));
			}
		}
		break;
		case 2009: // MacroRecord
		{
			if (_macroRecordEvent)
			{
				_macroRecordEvent(*this, *make_self<implementation::MacroRecordEventArgs>(static_cast<int32_t>(data->message), static_cast<int32_t>(data->wParam), static_cast<int32_t>(data->lParam)));
			}
		}
		break;
		case 2010: // MarginClick
		{
			if (_marginClickEvent)
			{
				_marginClickEvent(*this, *make_self<implementation::MarginClickEventArgs>(static_cast<int32_t>(data->modifiers), static_cast<int32_t>(data->position), static_cast<int32_t>(data->margin)));
			}
		}
		break;
		case 2011: // NeedShown
		{
			if (_needShownEvent)
			{
				_needShownEvent(*this, *make_self<implementation::NeedShownEventArgs>(static_cast<int32_t>(data->position), static_cast<int32_t>(data->length)));
			}
		}
		break;
		case 2013: // Painted
		{
			if (_paintedEvent)
			{
				_paintedEvent(*this, *make_self<implementation::PaintedEventArgs>());
			}
		}
		break;
		case 2014: // UserListSelection
		{
			if (_userListSelectionEvent)
			{
				_userListSelectionEvent(*this, *make_self<implementation::UserListSelectionEventArgs>(static_cast<int32_t>(data->listType), data->text, static_cast<int32_t>(data->position), static_cast<int32_t>(data->ch), static_cast<MicaEditor::CompletionMethods>(data->listCompletionMethod)));
			}
		}
		break;
		case 2015: // URIDropped
		{
			if (_uRIDroppedEvent)
			{
				_uRIDroppedEvent(*this, *make_self<implementation::URIDroppedEventArgs>(data->text));
			}
		}
		break;
		case 2016: // DwellStart
		{
			if (_dwellStartEvent)
			{
				_dwellStartEvent(*this, *make_self<implementation::DwellStartEventArgs>(static_cast<int32_t>(data->position), static_cast<int32_t>(data->x), static_cast<int32_t>(data->y)));
			}
		}
		break;
		case 2017: // DwellEnd
		{
			if (_dwellEndEvent)
			{
				_dwellEndEvent(*this, *make_self<implementation::DwellEndEventArgs>(static_cast<int32_t>(data->position), static_cast<int32_t>(data->x), static_cast<int32_t>(data->y)));
			}
		}
		break;
		case 2018: // ZoomChanged
		{
			if (_zoomChangedEvent)
			{
				_zoomChangedEvent(*this, *make_self<implementation::ZoomChangedEventArgs>());
			}
		}
		break;
		case 2019: // HotSpotClick
		{
			if (_hotSpotClickEvent)
			{
				_hotSpotClickEvent(*this, *make_self<implementation::HotSpotClickEventArgs>(static_cast<int32_t>(data->modifiers), static_cast<int32_t>(data->position)));
			}
		}
		break;
		case 2020: // HotSpotDoubleClick
		{
			if (_hotSpotDoubleClickEvent)
			{
				_hotSpotDoubleClickEvent(*this, *make_self<implementation::HotSpotDoubleClickEventArgs>(static_cast<int32_t>(data->modifiers), static_cast<int32_t>(data->position)));
			}
		}
		break;
		case 2021: // CallTipClick
		{
			if (_callTipClickEvent)
			{
				_callTipClickEvent(*this, *make_self<implementation::CallTipClickEventArgs>(static_cast<int32_t>(data->position)));
			}
		}
		break;
		case 2022: // AutoCSelection
		{
			if (_autoCSelectionEvent)
			{
				_autoCSelectionEvent(*this, *make_self<implementation::AutoCSelectionEventArgs>(data->text, static_cast<int32_t>(data->position), static_cast<int32_t>(data->ch), static_cast<MicaEditor::CompletionMethods>(data->listCompletionMethod)));
			}
		}
		break;
		case 2023: // IndicatorClick
		{
			if (_indicatorClickEvent)
			{
				_indicatorClickEvent(*this, *make_self<implementation::IndicatorClickEventArgs>(static_cast<int32_t>(data->modifiers), static_cast<int32_t>(data->position)));
			}
		}
		break;
		case 2024: // IndicatorRelease
		{
			if (_indicatorReleaseEvent)
			{
				_indicatorReleaseEvent(*this, *make_self<implementation::IndicatorReleaseEventArgs>(static_cast<int32_t>(data->modifiers), static_cast<int32_t>(data->position)));
			}
		}
		break;
		case 2025: // AutoCCancelled
		{
			if (_autoCCancelledEvent)
			{
				_autoCCancelledEvent(*this, *make_self<implementation::AutoCCancelledEventArgs>());
			}
		}
		break;
		case 2026: // AutoCCharDeleted
		{
			if (_autoCCharDeletedEvent)
			{
				_autoCCharDeletedEvent(*this, *make_self<implementation::AutoCCharDeletedEventArgs>());
			}
		}
		break;
		case 2027: // HotSpotReleaseClick
		{
			if (_hotSpotReleaseClickEvent)
			{
				_hotSpotReleaseClickEvent(*this, *make_self<implementation::HotSpotReleaseClickEventArgs>(static_cast<int32_t>(data->modifiers), static_cast<int32_t>(data->position)));
			}
		}
		break;
		case 2028: // FocusIn
		{
			if (_focusInEvent)
			{
				_focusInEvent(*this, *make_self<implementation::FocusInEventArgs>());
			}
		}
		break;
		case 2029: // FocusOut
		{
			if (_focusOutEvent)
			{
				_focusOutEvent(*this, *make_self<implementation::FocusOutEventArgs>());
			}
		}
		break;
		case 2030: // AutoCCompleted
		{
			if (_autoCCompletedEvent)
			{
				_autoCCompletedEvent(*this, *make_self<implementation::AutoCCompletedEventArgs>(data->text, static_cast<int32_t>(data->position), static_cast<int32_t>(data->ch), static_cast<MicaEditor::CompletionMethods>(data->listCompletionMethod)));
			}
		}
		break;
		case 2031: // MarginRightClick
		{
			if (_marginRightClickEvent)
			{
				_marginRightClickEvent(*this, *make_self<implementation::MarginRightClickEventArgs>(static_cast<int32_t>(data->modifiers), static_cast<int32_t>(data->position), static_cast<int32_t>(data->margin)));
			}
		}
		break;
		case 2032: // AutoCSelectionChange
		{
			if (_autoCSelectionChangeEvent)
			{
				_autoCSelectionChangeEvent(*this, *make_self<implementation::AutoCSelectionChangeEventArgs>(static_cast<int32_t>(data->listType), data->text, static_cast<int32_t>(data->position)));
			}
		}
		break;
		}
	}

	event_token Editor::StyleNeeded(MicaEditor::StyleNeededHandler const &handler)
	{
		return _styleNeededEvent.add(handler);
	}

	void Editor::StyleNeeded(event_token const &token)
	{
		_styleNeededEvent.remove(token);
	}

	event_token Editor::CharAdded(MicaEditor::CharAddedHandler const &handler)
	{
		return _charAddedEvent.add(handler);
	}

	void Editor::CharAdded(event_token const &token)
	{
		_charAddedEvent.remove(token);
	}

	event_token Editor::SavePointReached(MicaEditor::SavePointReachedHandler const &handler)
	{
		return _savePointReachedEvent.add(handler);
	}

	void Editor::SavePointReached(event_token const &token)
	{
		_savePointReachedEvent.remove(token);
	}

	event_token Editor::SavePointLeft(MicaEditor::SavePointLeftHandler const &handler)
	{
		return _savePointLeftEvent.add(handler);
	}

	void Editor::SavePointLeft(event_token const &token)
	{
		_savePointLeftEvent.remove(token);
	}

	event_token Editor::ModifyAttemptRO(MicaEditor::ModifyAttemptROHandler const &handler)
	{
		return _modifyAttemptROEvent.add(handler);
	}

	void Editor::ModifyAttemptRO(event_token const &token)
	{
		_modifyAttemptROEvent.remove(token);
	}

	event_token Editor::Key(MicaEditor::KeyHandler const &handler)
	{
		return _keyEvent.add(handler);
	}

	void Editor::Key(event_token const &token)
	{
		_keyEvent.remove(token);
	}

	event_token Editor::DoubleClick(MicaEditor::DoubleClickHandler const &handler)
	{
		return _doubleClickEvent.add(handler);
	}

	void Editor::DoubleClick(event_token const &token)
	{
		_doubleClickEvent.remove(token);
	}

	event_token Editor::UpdateUI(MicaEditor::UpdateUIHandler const &handler)
	{
		return _updateUIEvent.add(handler);
	}

	void Editor::UpdateUI(event_token const &token)
	{
		_updateUIEvent.remove(token);
	}

	event_token Editor::Modified(MicaEditor::ModifiedHandler const &handler)
	{
		return _modifiedEvent.add(handler);
	}

	void Editor::Modified(event_token const &token)
	{
		_modifiedEvent.remove(token);
	}

	event_token Editor::MacroRecord(MicaEditor::MacroRecordHandler const &handler)
	{
		return _macroRecordEvent.add(handler);
	}

	void Editor::MacroRecord(event_token const &token)
	{
		_macroRecordEvent.remove(token);
	}

	event_token Editor::MarginClick(MicaEditor::MarginClickHandler const &handler)
	{
		return _marginClickEvent.add(handler);
	}

	void Editor::MarginClick(event_token const &token)
	{
		_marginClickEvent.remove(token);
	}

	event_token Editor::NeedShown(MicaEditor::NeedShownHandler const &handler)
	{
		return _needShownEvent.add(handler);
	}

	void Editor::NeedShown(event_token const &token)
	{
		_needShownEvent.remove(token);
	}

	event_token Editor::Painted(MicaEditor::PaintedHandler const &handler)
	{
		return _paintedEvent.add(handler);
	}

	void Editor::Painted(event_token const &token)
	{
		_paintedEvent.remove(token);
	}

	event_token Editor::UserListSelection(MicaEditor::UserListSelectionHandler const &handler)
	{
		return _userListSelectionEvent.add(handler);
	}

	void Editor::UserListSelection(event_token const &token)
	{
		_userListSelectionEvent.remove(token);
	}

	event_token Editor::URIDropped(MicaEditor::URIDroppedHandler const &handler)
	{
		return _uRIDroppedEvent.add(handler);
	}

	void Editor::URIDropped(event_token const &token)
	{
		_uRIDroppedEvent.remove(token);
	}

	event_token Editor::DwellStart(MicaEditor::DwellStartHandler const &handler)
	{
		return _dwellStartEvent.add(handler);
	}

	void Editor::DwellStart(event_token const &token)
	{
		_dwellStartEvent.remove(token);
	}

	event_token Editor::DwellEnd(MicaEditor::DwellEndHandler const &handler)
	{
		return _dwellEndEvent.add(handler);
	}

	void Editor::DwellEnd(event_token const &token)
	{
		_dwellEndEvent.remove(token);
	}

	event_token Editor::ZoomChanged(MicaEditor::ZoomChangedHandler const &handler)
	{
		return _zoomChangedEvent.add(handler);
	}

	void Editor::ZoomChanged(event_token const &token)
	{
		_zoomChangedEvent.remove(token);
	}

	event_token Editor::HotSpotClick(MicaEditor::HotSpotClickHandler const &handler)
	{
		return _hotSpotClickEvent.add(handler);
	}

	void Editor::HotSpotClick(event_token const &token)
	{
		_hotSpotClickEvent.remove(token);
	}

	event_token Editor::HotSpotDoubleClick(MicaEditor::HotSpotDoubleClickHandler const &handler)
	{
		return _hotSpotDoubleClickEvent.add(handler);
	}

	void Editor::HotSpotDoubleClick(event_token const &token)
	{
		_hotSpotDoubleClickEvent.remove(token);
	}

	event_token Editor::CallTipClick(MicaEditor::CallTipClickHandler const &handler)
	{
		return _callTipClickEvent.add(handler);
	}

	void Editor::CallTipClick(event_token const &token)
	{
		_callTipClickEvent.remove(token);
	}

	event_token Editor::AutoCSelection(MicaEditor::AutoCSelectionHandler const &handler)
	{
		return _autoCSelectionEvent.add(handler);
	}

	void Editor::AutoCSelection(event_token const &token)
	{
		_autoCSelectionEvent.remove(token);
	}

	event_token Editor::IndicatorClick(MicaEditor::IndicatorClickHandler const &handler)
	{
		return _indicatorClickEvent.add(handler);
	}

	void Editor::IndicatorClick(event_token const &token)
	{
		_indicatorClickEvent.remove(token);
	}

	event_token Editor::IndicatorRelease(MicaEditor::IndicatorReleaseHandler const &handler)
	{
		return _indicatorReleaseEvent.add(handler);
	}

	void Editor::IndicatorRelease(event_token const &token)
	{
		_indicatorReleaseEvent.remove(token);
	}

	event_token Editor::AutoCCancelled(MicaEditor::AutoCCancelledHandler const &handler)
	{
		return _autoCCancelledEvent.add(handler);
	}

	void Editor::AutoCCancelled(event_token const &token)
	{
		_autoCCancelledEvent.remove(token);
	}

	event_token Editor::AutoCCharDeleted(MicaEditor::AutoCCharDeletedHandler const &handler)
	{
		return _autoCCharDeletedEvent.add(handler);
	}

	void Editor::AutoCCharDeleted(event_token const &token)
	{
		_autoCCharDeletedEvent.remove(token);
	}

	event_token Editor::HotSpotReleaseClick(MicaEditor::HotSpotReleaseClickHandler const &handler)
	{
		return _hotSpotReleaseClickEvent.add(handler);
	}

	void Editor::HotSpotReleaseClick(event_token const &token)
	{
		_hotSpotReleaseClickEvent.remove(token);
	}

	event_token Editor::FocusIn(MicaEditor::FocusInHandler const &handler)
	{
		return _focusInEvent.add(handler);
	}

	void Editor::FocusIn(event_token const &token)
	{
		_focusInEvent.remove(token);
	}

	event_token Editor::FocusOut(MicaEditor::FocusOutHandler const &handler)
	{
		return _focusOutEvent.add(handler);
	}

	void Editor::FocusOut(event_token const &token)
	{
		_focusOutEvent.remove(token);
	}

	event_token Editor::AutoCCompleted(MicaEditor::AutoCCompletedHandler const &handler)
	{
		return _autoCCompletedEvent.add(handler);
	}

	void Editor::AutoCCompleted(event_token const &token)
	{
		_autoCCompletedEvent.remove(token);
	}

	event_token Editor::MarginRightClick(MicaEditor::MarginRightClickHandler const &handler)
	{
		return _marginRightClickEvent.add(handler);
	}

	void Editor::MarginRightClick(event_token const &token)
	{
		_marginRightClickEvent.remove(token);
	}

	event_token Editor::AutoCSelectionChange(MicaEditor::AutoCSelectionChangeHandler const &handler)
	{
		return _autoCSelectionChangeEvent.add(handler);
	}

	void Editor::AutoCSelectionChange(event_token const &token)
	{
		_autoCSelectionChangeEvent.remove(token);
	}

	/*
	 * Returns the number of bytes in the document.
	 */
	int64_t Editor::Length()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetLength, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Returns the position of the caret.
	 */
	int64_t Editor::CurrentPos()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetCurrentPos, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the position of the caret.
	 */
	void Editor::CurrentPos(int64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCurrentPos, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Returns the position of the opposite end of the selection to the caret.
	 */
	int64_t Editor::Anchor()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetAnchor, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the selection anchor to a position. The anchor is the opposite
	 * end of the selection from the caret.
	 */
	void Editor::Anchor(int64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetAnchor, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Is undo history being collected?
	 */
	bool Editor::UndoCollection()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetUndoCollection, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Choose between collecting actions into the undo
	 * history and discarding them.
	 */
	void Editor::UndoCollection(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetUndoCollection, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Are white space characters currently visible?
	 * Returns one of SCWS_* constants.
	 */
	MicaEditor::WhiteSpace Editor::ViewWS()
	{
		return static_cast<MicaEditor::WhiteSpace>(_scintilla->WndProc(Scintilla::Message::GetViewWS, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Make white space characters invisible, always visible or visible outside indentation.
	 */
	void Editor::ViewWS(MicaEditor::WhiteSpace const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetViewWS, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the current tab draw mode.
	 * Returns one of SCTD_* constants.
	 */
	MicaEditor::TabDrawMode Editor::TabDrawMode()
	{
		return static_cast<MicaEditor::TabDrawMode>(_scintilla->WndProc(Scintilla::Message::GetTabDrawMode, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set how tabs are drawn when visible.
	 */
	void Editor::TabDrawMode(MicaEditor::TabDrawMode const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetTabDrawMode, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the position of the last correctly styled character.
	 */
	int64_t Editor::EndStyled()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetEndStyled, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the current end of line mode - one of CRLF, CR, or LF.
	 */
	MicaEditor::EndOfLine Editor::EOLMode()
	{
		return static_cast<MicaEditor::EndOfLine>(_scintilla->WndProc(Scintilla::Message::GetEOLMode, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the current end of line mode.
	 */
	void Editor::EOLMode(MicaEditor::EndOfLine const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetEOLMode, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Is drawing done first into a buffer or direct to the screen?
	 */
	bool Editor::BufferedDraw()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetBufferedDraw, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * If drawing is buffered then each line of text is drawn into a bitmap buffer
	 * before drawing it to the screen to avoid flicker.
	 */
	void Editor::BufferedDraw(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetBufferedDraw, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the visible size of a tab.
	 */
	int32_t Editor::TabWidth()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetTabWidth, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Change the visible size of a tab to be a multiple of the width of a space character.
	 */
	void Editor::TabWidth(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetTabWidth, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the minimum visual width of a tab.
	 */
	int32_t Editor::TabMinimumWidth()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetTabMinimumWidth, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the minimum visual width of a tab.
	 */
	void Editor::TabMinimumWidth(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetTabMinimumWidth, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Is the IME displayed in a window or inline?
	 */
	MicaEditor::IMEInteraction Editor::IMEInteraction()
	{
		return static_cast<MicaEditor::IMEInteraction>(_scintilla->WndProc(Scintilla::Message::GetIMEInteraction, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Choose to display the IME in a window or inline.
	 */
	void Editor::IMEInteraction(MicaEditor::IMEInteraction const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetIMEInteraction, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * How many margins are there?.
	 */
	int32_t Editor::Margins()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetMargins, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Allocate a non-standard number of margins.
	 */
	void Editor::Margins(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetMargins, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the alpha of the selection.
	 */
	MicaEditor::Alpha Editor::SelAlpha()
	{
		return static_cast<MicaEditor::Alpha>(_scintilla->WndProc(Scintilla::Message::GetSelAlpha, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the alpha of the selection.
	 */
	void Editor::SelAlpha(MicaEditor::Alpha const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetSelAlpha, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Is the selection end of line filled?
	 */
	bool Editor::SelEOLFilled()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetSelEOLFilled, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the selection to have its end of line filled or not.
	 */
	void Editor::SelEOLFilled(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetSelEOLFilled, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the layer for drawing selections
	 */
	MicaEditor::Layer Editor::SelectionLayer()
	{
		return static_cast<MicaEditor::Layer>(_scintilla->WndProc(Scintilla::Message::GetSelectionLayer, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the layer for drawing selections: either opaquely on base layer or translucently over text
	 */
	void Editor::SelectionLayer(MicaEditor::Layer const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetSelectionLayer, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the layer of the background of the line containing the caret.
	 */
	MicaEditor::Layer Editor::CaretLineLayer()
	{
		return static_cast<MicaEditor::Layer>(_scintilla->WndProc(Scintilla::Message::GetCaretLineLayer, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the layer of the background of the line containing the caret.
	 */
	void Editor::CaretLineLayer(MicaEditor::Layer const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCaretLineLayer, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get only highlighting subline instead of whole line.
	 */
	bool Editor::CaretLineHighlightSubLine()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetCaretLineHighlightSubLine, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set only highlighting subline instead of whole line.
	 */
	void Editor::CaretLineHighlightSubLine(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCaretLineHighlightSubLine, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the time in milliseconds that the caret is on and off.
	 */
	int32_t Editor::CaretPeriod()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetCaretPeriod, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the time in milliseconds that the caret is on and off. 0 = steady on.
	 */
	void Editor::CaretPeriod(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCaretPeriod, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the number of characters to have directly indexed categories
	 */
	int32_t Editor::CharacterCategoryOptimization()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetCharacterCategoryOptimization, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the number of characters to have directly indexed categories
	 */
	void Editor::CharacterCategoryOptimization(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCharacterCategoryOptimization, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the size of the dots used to mark space characters.
	 */
	int32_t Editor::WhitespaceSize()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetWhitespaceSize, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the size of the dots used to mark space characters.
	 */
	void Editor::WhitespaceSize(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetWhitespaceSize, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the last line number that has line state.
	 */
	int32_t Editor::MaxLineState()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetMaxLineState, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Is the background of the line containing the caret in a different colour?
	 */
	bool Editor::CaretLineVisible()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetCaretLineVisible, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Display the background of the line containing the caret in a different colour.
	 */
	void Editor::CaretLineVisible(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCaretLineVisible, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the colour of the background of the line containing the caret.
	 */
	int32_t Editor::CaretLineBack()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetCaretLineBack, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the colour of the background of the line containing the caret.
	 */
	void Editor::CaretLineBack(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCaretLineBack, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the caret line frame width.
	 * Width = 0 means this option is disabled.
	 */
	int32_t Editor::CaretLineFrame()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetCaretLineFrame, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Display the caret line framed.
	 * Set width != 0 to enable this option and width = 0 to disable it.
	 */
	void Editor::CaretLineFrame(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCaretLineFrame, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the auto-completion list separator character.
	 */
	int32_t Editor::AutoCSeparator()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::AutoCGetSeparator, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Change the separator character in the string setting up an auto-completion list.
	 * Default is space but can be changed if items contain space.
	 */
	void Editor::AutoCSeparator(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSetSeparator, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve whether auto-completion cancelled by backspacing before start.
	 */
	bool Editor::AutoCCancelAtStart()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::AutoCGetCancelAtStart, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Should the auto-completion list be cancelled if the user backspaces to a
	 * position before where the box was created.
	 */
	void Editor::AutoCCancelAtStart(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSetCancelAtStart, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve whether a single item auto-completion list automatically choose the item.
	 */
	bool Editor::AutoCChooseSingle()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::AutoCGetChooseSingle, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Should a single item auto-completion list automatically choose the item.
	 */
	void Editor::AutoCChooseSingle(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSetChooseSingle, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve state of ignore case flag.
	 */
	bool Editor::AutoCIgnoreCase()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::AutoCGetIgnoreCase, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set whether case is significant when performing auto-completion searches.
	 */
	void Editor::AutoCIgnoreCase(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSetIgnoreCase, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve whether or not autocompletion is hidden automatically when nothing matches.
	 */
	bool Editor::AutoCAutoHide()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::AutoCGetAutoHide, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set whether or not autocompletion is hidden automatically when nothing matches.
	 */
	void Editor::AutoCAutoHide(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSetAutoHide, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve autocompletion options.
	 */
	MicaEditor::AutoCompleteOption Editor::AutoCOptions()
	{
		return static_cast<MicaEditor::AutoCompleteOption>(_scintilla->WndProc(Scintilla::Message::AutoCGetOptions, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set autocompletion options.
	 */
	void Editor::AutoCOptions(MicaEditor::AutoCompleteOption const &value)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSetOptions, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve whether or not autocompletion deletes any word characters
	 * after the inserted text upon completion.
	 */
	bool Editor::AutoCDropRestOfWord()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::AutoCGetDropRestOfWord, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set whether or not autocompletion deletes any word characters
	 * after the inserted text upon completion.
	 */
	void Editor::AutoCDropRestOfWord(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSetDropRestOfWord, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the auto-completion list type-separator character.
	 */
	int32_t Editor::AutoCTypeSeparator()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::AutoCGetTypeSeparator, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Change the type-separator character in the string setting up an auto-completion list.
	 * Default is '?' but can be changed if items contain '?'.
	 */
	void Editor::AutoCTypeSeparator(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSetTypeSeparator, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the maximum width, in characters, of auto-completion and user lists.
	 */
	int32_t Editor::AutoCMaxWidth()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::AutoCGetMaxWidth, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the maximum width, in characters, of auto-completion and user lists.
	 * Set to 0 to autosize to fit longest item, which is the default.
	 */
	void Editor::AutoCMaxWidth(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSetMaxWidth, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the maximum height, in rows, of auto-completion and user lists.
	 */
	int32_t Editor::AutoCMaxHeight()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::AutoCGetMaxHeight, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the maximum height, in rows, of auto-completion and user lists.
	 * The default is 5 rows.
	 */
	void Editor::AutoCMaxHeight(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSetMaxHeight, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve indentation size.
	 */
	int32_t Editor::Indent()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetIndent, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the number of spaces used for one level of indentation.
	 */
	void Editor::Indent(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetIndent, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve whether tabs will be used in indentation.
	 */
	bool Editor::UseTabs()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetUseTabs, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Indentation will only use space characters if useTabs is false, otherwise
	 * it will use a combination of tabs and spaces.
	 */
	void Editor::UseTabs(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetUseTabs, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Is the horizontal scroll bar visible?
	 */
	bool Editor::HScrollBar()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetHScrollBar, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Show or hide the horizontal scroll bar.
	 */
	void Editor::HScrollBar(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetHScrollBar, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Are the indentation guides visible?
	 */
	MicaEditor::IndentView Editor::IndentationGuides()
	{
		return static_cast<MicaEditor::IndentView>(_scintilla->WndProc(Scintilla::Message::GetIndentationGuides, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Show or hide indentation guides.
	 */
	void Editor::IndentationGuides(MicaEditor::IndentView const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetIndentationGuides, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the highlighted indentation guide column.
	 */
	int64_t Editor::HighlightGuide()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetHighlightGuide, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the highlighted indentation guide column.
	 * 0 = no highlighted guide.
	 */
	void Editor::HighlightGuide(int64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetHighlightGuide, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the code page used to interpret the bytes of the document as characters.
	 */
	int32_t Editor::CodePage()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetCodePage, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the code page used to interpret the bytes of the document as characters.
	 * The SC_CP_UTF8 value can be used to enter Unicode mode.
	 */
	void Editor::CodePage(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCodePage, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the foreground colour of the caret.
	 */
	int32_t Editor::CaretFore()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetCaretFore, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the foreground colour of the caret.
	 */
	void Editor::CaretFore(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCaretFore, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * In read-only mode?
	 */
	bool Editor::ReadOnly()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetReadOnly, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set to read only or read write.
	 */
	void Editor::ReadOnly(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetReadOnly, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Returns the position at the start of the selection.
	 */
	int64_t Editor::SelectionStart()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetSelectionStart, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the position that starts the selection - this becomes the anchor.
	 */
	void Editor::SelectionStart(int64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetSelectionStart, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Returns the position at the end of the selection.
	 */
	int64_t Editor::SelectionEnd()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetSelectionEnd, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the position that ends the selection - this becomes the caret.
	 */
	void Editor::SelectionEnd(int64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetSelectionEnd, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Returns the print magnification.
	 */
	int32_t Editor::PrintMagnification()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetPrintMagnification, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the print magnification added to the point size of each style for printing.
	 */
	void Editor::PrintMagnification(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetPrintMagnification, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Returns the print colour mode.
	 */
	MicaEditor::PrintOption Editor::PrintColourMode()
	{
		return static_cast<MicaEditor::PrintOption>(_scintilla->WndProc(Scintilla::Message::GetPrintColourMode, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Modify colours when printing for clearer printed text.
	 */
	void Editor::PrintColourMode(MicaEditor::PrintOption const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetPrintColourMode, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Report change history status.
	 */
	MicaEditor::ChangeHistoryOption Editor::ChangeHistory()
	{
		return static_cast<MicaEditor::ChangeHistoryOption>(_scintilla->WndProc(Scintilla::Message::GetChangeHistory, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Enable or disable change history.
	 */
	void Editor::ChangeHistory(MicaEditor::ChangeHistoryOption const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetChangeHistory, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the display line at the top of the display.
	 */
	int64_t Editor::FirstVisibleLine()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetFirstVisibleLine, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Scroll so that a display line is at the top of the display.
	 */
	void Editor::FirstVisibleLine(int64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetFirstVisibleLine, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Returns the number of lines in the document. There is always at least one.
	 */
	int64_t Editor::LineCount()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetLineCount, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Returns the size in pixels of the left margin.
	 */
	int32_t Editor::MarginLeft()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetMarginLeft, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the size in pixels of the left margin.
	 */
	void Editor::MarginLeft(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetMarginLeft, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(value));
	}

	/*
	 * Returns the size in pixels of the right margin.
	 */
	int32_t Editor::MarginRight()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetMarginRight, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the size in pixels of the right margin.
	 */
	void Editor::MarginRight(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetMarginRight, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(value));
	}

	/*
	 * Is the document different from when it was last saved?
	 */
	bool Editor::Modify()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetModify, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	bool Editor::SelectionHidden()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetSelectionHidden, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the number of characters in the document.
	 */
	int64_t Editor::TextLength()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetTextLength, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve a pointer to a function that processes messages for this Scintilla.
	 */
	uint64_t Editor::DirectFunction()
	{
		return static_cast<uint64_t>(_scintilla->WndProc(Scintilla::Message::GetDirectFunction, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve a pointer to a function that processes messages for this Scintilla and returns status.
	 */
	uint64_t Editor::DirectStatusFunction()
	{
		return static_cast<uint64_t>(_scintilla->WndProc(Scintilla::Message::GetDirectStatusFunction, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve a pointer value to use as the first argument when calling
	 * the function returned by GetDirectFunction.
	 */
	uint64_t Editor::DirectPointer()
	{
		return static_cast<uint64_t>(_scintilla->WndProc(Scintilla::Message::GetDirectPointer, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Returns true if overtype mode is active otherwise false is returned.
	 */
	bool Editor::Overtype()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetOvertype, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set to overtype (true) or insert mode.
	 */
	void Editor::Overtype(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetOvertype, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Returns the width of the insert mode caret.
	 */
	int32_t Editor::CaretWidth()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetCaretWidth, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the width of the insert mode caret.
	 */
	void Editor::CaretWidth(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCaretWidth, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the position that starts the target.
	 */
	int64_t Editor::TargetStart()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetTargetStart, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the position that starts the target which is used for updating the
	 * document without affecting the scroll position.
	 */
	void Editor::TargetStart(int64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetTargetStart, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the virtual space of the target start
	 */
	int64_t Editor::TargetStartVirtualSpace()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetTargetStartVirtualSpace, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the virtual space of the target start
	 */
	void Editor::TargetStartVirtualSpace(int64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetTargetStartVirtualSpace, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the position that ends the target.
	 */
	int64_t Editor::TargetEnd()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetTargetEnd, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the position that ends the target which is used for updating the
	 * document without affecting the scroll position.
	 */
	void Editor::TargetEnd(int64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetTargetEnd, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the virtual space of the target end
	 */
	int64_t Editor::TargetEndVirtualSpace()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetTargetEndVirtualSpace, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the virtual space of the target end
	 */
	void Editor::TargetEndVirtualSpace(int64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetTargetEndVirtualSpace, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the search flags used by SearchInTarget.
	 */
	MicaEditor::FindOption Editor::SearchFlags()
	{
		return static_cast<MicaEditor::FindOption>(_scintilla->WndProc(Scintilla::Message::GetSearchFlags, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the search flags used by SearchInTarget.
	 */
	void Editor::SearchFlags(MicaEditor::FindOption const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetSearchFlags, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Are all lines visible?
	 */
	bool Editor::AllLinesVisible()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetAllLinesVisible, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the style of fold display text.
	 */
	MicaEditor::FoldDisplayTextStyle Editor::FoldDisplayTextStyle()
	{
		return static_cast<MicaEditor::FoldDisplayTextStyle>(_scintilla->WndProc(Scintilla::Message::FoldDisplayTextGetStyle, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the style of fold display text.
	 */
	void Editor::FoldDisplayTextStyle(MicaEditor::FoldDisplayTextStyle const &value)
	{
		_scintilla->WndProc(Scintilla::Message::FoldDisplayTextSetStyle, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get automatic folding behaviours.
	 */
	MicaEditor::AutomaticFold Editor::AutomaticFold()
	{
		return static_cast<MicaEditor::AutomaticFold>(_scintilla->WndProc(Scintilla::Message::GetAutomaticFold, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set automatic folding behaviours.
	 */
	void Editor::AutomaticFold(MicaEditor::AutomaticFold const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetAutomaticFold, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Does a tab pressed when caret is within indentation indent?
	 */
	bool Editor::TabIndents()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetTabIndents, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets whether a tab pressed when caret is within indentation indents.
	 */
	void Editor::TabIndents(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetTabIndents, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Does a backspace pressed when caret is within indentation unindent?
	 */
	bool Editor::BackSpaceUnIndents()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetBackSpaceUnIndents, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets whether a backspace pressed when caret is within indentation unindents.
	 */
	void Editor::BackSpaceUnIndents(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetBackSpaceUnIndents, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the time the mouse must sit still to generate a mouse dwell event.
	 */
	int32_t Editor::MouseDwellTime()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetMouseDwellTime, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the time the mouse must sit still to generate a mouse dwell event.
	 */
	void Editor::MouseDwellTime(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetMouseDwellTime, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the limits to idle styling.
	 */
	MicaEditor::IdleStyling Editor::IdleStyling()
	{
		return static_cast<MicaEditor::IdleStyling>(_scintilla->WndProc(Scintilla::Message::GetIdleStyling, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets limits to idle styling.
	 */
	void Editor::IdleStyling(MicaEditor::IdleStyling const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetIdleStyling, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve whether text is word wrapped.
	 */
	MicaEditor::Wrap Editor::WrapMode()
	{
		return static_cast<MicaEditor::Wrap>(_scintilla->WndProc(Scintilla::Message::GetWrapMode, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets whether text is word wrapped.
	 */
	void Editor::WrapMode(MicaEditor::Wrap const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetWrapMode, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrive the display mode of visual flags for wrapped lines.
	 */
	MicaEditor::WrapVisualFlag Editor::WrapVisualFlags()
	{
		return static_cast<MicaEditor::WrapVisualFlag>(_scintilla->WndProc(Scintilla::Message::GetWrapVisualFlags, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the display mode of visual flags for wrapped lines.
	 */
	void Editor::WrapVisualFlags(MicaEditor::WrapVisualFlag const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetWrapVisualFlags, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrive the location of visual flags for wrapped lines.
	 */
	MicaEditor::WrapVisualLocation Editor::WrapVisualFlagsLocation()
	{
		return static_cast<MicaEditor::WrapVisualLocation>(_scintilla->WndProc(Scintilla::Message::GetWrapVisualFlagsLocation, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the location of visual flags for wrapped lines.
	 */
	void Editor::WrapVisualFlagsLocation(MicaEditor::WrapVisualLocation const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetWrapVisualFlagsLocation, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrive the start indent for wrapped lines.
	 */
	int32_t Editor::WrapStartIndent()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetWrapStartIndent, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the start indent for wrapped lines.
	 */
	void Editor::WrapStartIndent(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetWrapStartIndent, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve how wrapped sublines are placed. Default is fixed.
	 */
	MicaEditor::WrapIndentMode Editor::WrapIndentMode()
	{
		return static_cast<MicaEditor::WrapIndentMode>(_scintilla->WndProc(Scintilla::Message::GetWrapIndentMode, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets how wrapped sublines are placed. Default is fixed.
	 */
	void Editor::WrapIndentMode(MicaEditor::WrapIndentMode const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetWrapIndentMode, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the degree of caching of layout information.
	 */
	MicaEditor::LineCache Editor::LayoutCache()
	{
		return static_cast<MicaEditor::LineCache>(_scintilla->WndProc(Scintilla::Message::GetLayoutCache, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the degree of caching of layout information.
	 */
	void Editor::LayoutCache(MicaEditor::LineCache const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetLayoutCache, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the document width assumed for scrolling.
	 */
	int32_t Editor::ScrollWidth()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetScrollWidth, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the document width assumed for scrolling.
	 */
	void Editor::ScrollWidth(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetScrollWidth, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve whether the scroll width tracks wide lines.
	 */
	bool Editor::ScrollWidthTracking()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetScrollWidthTracking, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets whether the maximum width line displayed is used to set scroll width.
	 */
	void Editor::ScrollWidthTracking(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetScrollWidthTracking, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve whether the maximum scroll position has the last
	 * line at the bottom of the view.
	 */
	bool Editor::EndAtLastLine()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetEndAtLastLine, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the scroll range so that maximum scroll position has
	 * the last line at the bottom of the view (default).
	 * Setting this to false allows scrolling one page below the last line.
	 */
	void Editor::EndAtLastLine(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetEndAtLastLine, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Is the vertical scroll bar visible?
	 */
	bool Editor::VScrollBar()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetVScrollBar, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Show or hide the vertical scroll bar.
	 */
	void Editor::VScrollBar(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetVScrollBar, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * How many phases is drawing done in?
	 */
	MicaEditor::PhasesDraw Editor::PhasesDraw()
	{
		return static_cast<MicaEditor::PhasesDraw>(_scintilla->WndProc(Scintilla::Message::GetPhasesDraw, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * In one phase draw, text is drawn in a series of rectangular blocks with no overlap.
	 * In two phase draw, text is drawn in a series of lines allowing runs to overlap horizontally.
	 * In multiple phase draw, each element is drawn over the whole drawing area, allowing text
	 * to overlap from one line to the next.
	 */
	void Editor::PhasesDraw(MicaEditor::PhasesDraw const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetPhasesDraw, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the quality level for text.
	 */
	MicaEditor::FontQuality Editor::FontQuality()
	{
		return static_cast<MicaEditor::FontQuality>(_scintilla->WndProc(Scintilla::Message::GetFontQuality, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Choose the quality level for text from the FontQuality enumeration.
	 */
	void Editor::FontQuality(MicaEditor::FontQuality const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetFontQuality, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the effect of pasting when there are multiple selections.
	 */
	MicaEditor::MultiPaste Editor::MultiPaste()
	{
		return static_cast<MicaEditor::MultiPaste>(_scintilla->WndProc(Scintilla::Message::GetMultiPaste, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Change the effect of pasting when there are multiple selections.
	 */
	void Editor::MultiPaste(MicaEditor::MultiPaste const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetMultiPaste, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Report accessibility status.
	 */
	MicaEditor::Accessibility Editor::Accessibility()
	{
		return static_cast<MicaEditor::Accessibility>(_scintilla->WndProc(Scintilla::Message::GetAccessibility, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Enable or disable accessibility.
	 */
	void Editor::Accessibility(MicaEditor::Accessibility const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetAccessibility, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Are the end of line characters visible?
	 */
	bool Editor::ViewEOL()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetViewEOL, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Make the end of line characters visible or invisible.
	 */
	void Editor::ViewEOL(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetViewEOL, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve a pointer to the document object.
	 */
	uint64_t Editor::DocPointer()
	{
		return static_cast<uint64_t>(_scintilla->WndProc(Scintilla::Message::GetDocPointer, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Change the document object used.
	 */
	void Editor::DocPointer(uint64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetDocPointer, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(value));
	}

	/*
	 * Retrieve the column number which text should be kept within.
	 */
	int64_t Editor::EdgeColumn()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetEdgeColumn, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the column number of the edge.
	 * If text goes past the edge then it is highlighted.
	 */
	void Editor::EdgeColumn(int64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetEdgeColumn, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the edge highlight mode.
	 */
	MicaEditor::EdgeVisualStyle Editor::EdgeMode()
	{
		return static_cast<MicaEditor::EdgeVisualStyle>(_scintilla->WndProc(Scintilla::Message::GetEdgeMode, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * The edge may be displayed by a line (EDGE_LINE/EDGE_MULTILINE) or by highlighting text that
	 * goes beyond it (EDGE_BACKGROUND) or not displayed at all (EDGE_NONE).
	 */
	void Editor::EdgeMode(MicaEditor::EdgeVisualStyle const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetEdgeMode, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the colour used in edge indication.
	 */
	int32_t Editor::EdgeColour()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetEdgeColour, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Change the colour used in edge indication.
	 */
	void Editor::EdgeColour(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetEdgeColour, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieves the number of lines completely visible.
	 */
	int64_t Editor::LinesOnScreen()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::LinesOnScreen, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Is the selection rectangular? The alternative is the more common stream selection.
	 */
	bool Editor::SelectionIsRectangle()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::SelectionIsRectangle, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the zoom level.
	 */
	int32_t Editor::Zoom()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetZoom, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the zoom level. This number of points is added to the size of all fonts.
	 * It may be positive to magnify or negative to reduce.
	 */
	void Editor::Zoom(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetZoom, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get which document options are set.
	 */
	MicaEditor::DocumentOption Editor::DocumentOptions()
	{
		return static_cast<MicaEditor::DocumentOption>(_scintilla->WndProc(Scintilla::Message::GetDocumentOptions, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get which document modification events are sent to the container.
	 */
	MicaEditor::ModificationFlags Editor::ModEventMask()
	{
		return static_cast<MicaEditor::ModificationFlags>(_scintilla->WndProc(Scintilla::Message::GetModEventMask, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set which document modification events are sent to the container.
	 */
	void Editor::ModEventMask(MicaEditor::ModificationFlags const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetModEventMask, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get whether command events are sent to the container.
	 */
	bool Editor::CommandEvents()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetCommandEvents, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set whether command events are sent to the container.
	 */
	void Editor::CommandEvents(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCommandEvents, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get internal focus flag.
	 */
	bool Editor::Focus()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetFocus, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Change internal focus flag.
	 */
	void Editor::Focus(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetFocus, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get error status.
	 */
	MicaEditor::Status Editor::Status()
	{
		return static_cast<MicaEditor::Status>(_scintilla->WndProc(Scintilla::Message::GetStatus, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Change error status - 0 = OK.
	 */
	void Editor::Status(MicaEditor::Status const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetStatus, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get whether mouse gets captured.
	 */
	bool Editor::MouseDownCaptures()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetMouseDownCaptures, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set whether the mouse is captured when its button is pressed.
	 */
	void Editor::MouseDownCaptures(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetMouseDownCaptures, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get whether mouse wheel can be active outside the window.
	 */
	bool Editor::MouseWheelCaptures()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetMouseWheelCaptures, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set whether the mouse wheel can be active outside the window.
	 */
	void Editor::MouseWheelCaptures(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetMouseWheelCaptures, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get cursor type.
	 */
	MicaEditor::CursorShape Editor::Cursor()
	{
		return static_cast<MicaEditor::CursorShape>(_scintilla->WndProc(Scintilla::Message::GetCursor, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the cursor to one of the SC_CURSOR* values.
	 */
	void Editor::Cursor(MicaEditor::CursorShape const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCursor, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the way control characters are displayed.
	 */
	int32_t Editor::ControlCharSymbol()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetControlCharSymbol, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Change the way control characters are displayed:
	 * If symbol is < 32, keep the drawn way, else, use the given character.
	 */
	void Editor::ControlCharSymbol(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetControlCharSymbol, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the xOffset (ie, horizontal scroll position).
	 */
	int32_t Editor::XOffset()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetXOffset, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the xOffset (ie, horizontal scroll position).
	 */
	void Editor::XOffset(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetXOffset, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Is printing line wrapped?
	 */
	MicaEditor::Wrap Editor::PrintWrapMode()
	{
		return static_cast<MicaEditor::Wrap>(_scintilla->WndProc(Scintilla::Message::GetPrintWrapMode, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set printing to line wrapped (SC_WRAP_WORD) or not line wrapped (SC_WRAP_NONE).
	 */
	void Editor::PrintWrapMode(MicaEditor::Wrap const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetPrintWrapMode, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get whether underlining for active hotspots.
	 */
	bool Editor::HotspotActiveUnderline()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetHotspotActiveUnderline, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Enable / Disable underlining active hotspots.
	 */
	void Editor::HotspotActiveUnderline(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetHotspotActiveUnderline, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the HotspotSingleLine property
	 */
	bool Editor::HotspotSingleLine()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetHotspotSingleLine, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Limit hotspots to single line so hotspots on two lines don't merge.
	 */
	void Editor::HotspotSingleLine(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetHotspotSingleLine, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the mode of the current selection.
	 */
	MicaEditor::SelectionMode Editor::SelectionMode()
	{
		return static_cast<MicaEditor::SelectionMode>(_scintilla->WndProc(Scintilla::Message::GetSelectionMode, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the selection mode to stream (SC_SEL_STREAM) or rectangular (SC_SEL_RECTANGLE/SC_SEL_THIN) or
	 * by lines (SC_SEL_LINES).
	 */
	void Editor::SelectionMode(MicaEditor::SelectionMode const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetSelectionMode, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get whether or not regular caret moves will extend or reduce the selection.
	 */
	bool Editor::MoveExtendsSelection()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetMoveExtendsSelection, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get currently selected item position in the auto-completion list
	 */
	int32_t Editor::AutoCCurrent()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::AutoCGetCurrent, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get auto-completion case insensitive behaviour.
	 */
	MicaEditor::CaseInsensitiveBehaviour Editor::AutoCCaseInsensitiveBehaviour()
	{
		return static_cast<MicaEditor::CaseInsensitiveBehaviour>(_scintilla->WndProc(Scintilla::Message::AutoCGetCaseInsensitiveBehaviour, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set auto-completion case insensitive behaviour to either prefer case-sensitive matches or have no preference.
	 */
	void Editor::AutoCCaseInsensitiveBehaviour(MicaEditor::CaseInsensitiveBehaviour const &value)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSetCaseInsensitiveBehaviour, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the effect of autocompleting when there are multiple selections.
	 */
	MicaEditor::MultiAutoComplete Editor::AutoCMulti()
	{
		return static_cast<MicaEditor::MultiAutoComplete>(_scintilla->WndProc(Scintilla::Message::AutoCGetMulti, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Change the effect of autocompleting when there are multiple selections.
	 */
	void Editor::AutoCMulti(MicaEditor::MultiAutoComplete const &value)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSetMulti, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the way autocompletion lists are ordered.
	 */
	MicaEditor::Ordering Editor::AutoCOrder()
	{
		return static_cast<MicaEditor::Ordering>(_scintilla->WndProc(Scintilla::Message::AutoCGetOrder, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the way autocompletion lists are ordered.
	 */
	void Editor::AutoCOrder(MicaEditor::Ordering const &value)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSetOrder, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Can the caret preferred x position only be changed by explicit movement commands?
	 */
	MicaEditor::CaretSticky Editor::CaretSticky()
	{
		return static_cast<MicaEditor::CaretSticky>(_scintilla->WndProc(Scintilla::Message::GetCaretSticky, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Stop the caret preferred x position changing when the user types.
	 */
	void Editor::CaretSticky(MicaEditor::CaretSticky const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCaretSticky, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get convert-on-paste setting
	 */
	bool Editor::PasteConvertEndings()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetPasteConvertEndings, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Enable/Disable convert-on-paste for line endings
	 */
	void Editor::PasteConvertEndings(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetPasteConvertEndings, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the background alpha of the caret line.
	 */
	MicaEditor::Alpha Editor::CaretLineBackAlpha()
	{
		return static_cast<MicaEditor::Alpha>(_scintilla->WndProc(Scintilla::Message::GetCaretLineBackAlpha, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set background alpha of the caret line.
	 */
	void Editor::CaretLineBackAlpha(MicaEditor::Alpha const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCaretLineBackAlpha, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Returns the current style of the caret.
	 */
	MicaEditor::CaretStyle Editor::CaretStyle()
	{
		return static_cast<MicaEditor::CaretStyle>(_scintilla->WndProc(Scintilla::Message::GetCaretStyle, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the style of the caret to be drawn.
	 */
	void Editor::CaretStyle(MicaEditor::CaretStyle const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCaretStyle, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the current indicator
	 */
	int32_t Editor::IndicatorCurrent()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetIndicatorCurrent, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the indicator used for IndicatorFillRange and IndicatorClearRange
	 */
	void Editor::IndicatorCurrent(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetIndicatorCurrent, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the current indicator value
	 */
	int32_t Editor::IndicatorValue()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetIndicatorValue, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the value used for IndicatorFillRange
	 */
	void Editor::IndicatorValue(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetIndicatorValue, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * How many entries are allocated to the position cache?
	 */
	int32_t Editor::PositionCache()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetPositionCache, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set number of entries in position cache
	 */
	void Editor::PositionCache(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetPositionCache, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get maximum number of threads used for layout
	 */
	int32_t Editor::LayoutThreads()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetLayoutThreads, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set maximum number of threads used for layout
	 */
	void Editor::LayoutThreads(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetLayoutThreads, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Compact the document buffer and return a read-only pointer to the
	 * characters in the document.
	 */
	uint64_t Editor::CharacterPointer()
	{
		return static_cast<uint64_t>(_scintilla->WndProc(Scintilla::Message::GetCharacterPointer, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Return a position which, to avoid performance costs, should not be within
	 * the range of a call to GetRangePointer.
	 */
	int64_t Editor::GapPosition()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetGapPosition, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get extra ascent for each line
	 */
	int32_t Editor::ExtraAscent()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetExtraAscent, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set extra ascent for each line
	 */
	void Editor::ExtraAscent(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetExtraAscent, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get extra descent for each line
	 */
	int32_t Editor::ExtraDescent()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetExtraDescent, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set extra descent for each line
	 */
	void Editor::ExtraDescent(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetExtraDescent, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the start of the range of style numbers used for margin text
	 */
	int32_t Editor::MarginStyleOffset()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::MarginGetStyleOffset, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the start of the range of style numbers used for margin text
	 */
	void Editor::MarginStyleOffset(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::MarginSetStyleOffset, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the margin options.
	 */
	MicaEditor::MarginOption Editor::MarginOptions()
	{
		return static_cast<MicaEditor::MarginOption>(_scintilla->WndProc(Scintilla::Message::GetMarginOptions, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the margin options.
	 */
	void Editor::MarginOptions(MicaEditor::MarginOption const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetMarginOptions, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the visibility for the annotations for a view
	 */
	MicaEditor::AnnotationVisible Editor::AnnotationVisible()
	{
		return static_cast<MicaEditor::AnnotationVisible>(_scintilla->WndProc(Scintilla::Message::AnnotationGetVisible, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the visibility for the annotations for a view
	 */
	void Editor::AnnotationVisible(MicaEditor::AnnotationVisible const &value)
	{
		_scintilla->WndProc(Scintilla::Message::AnnotationSetVisible, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the start of the range of style numbers used for annotations
	 */
	int32_t Editor::AnnotationStyleOffset()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::AnnotationGetStyleOffset, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the start of the range of style numbers used for annotations
	 */
	void Editor::AnnotationStyleOffset(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::AnnotationSetStyleOffset, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Whether switching to rectangular mode while selecting with the mouse is allowed.
	 */
	bool Editor::MouseSelectionRectangularSwitch()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetMouseSelectionRectangularSwitch, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set whether switching to rectangular mode while selecting with the mouse is allowed.
	 */
	void Editor::MouseSelectionRectangularSwitch(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetMouseSelectionRectangularSwitch, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Whether multiple selections can be made
	 */
	bool Editor::MultipleSelection()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetMultipleSelection, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set whether multiple selections can be made
	 */
	void Editor::MultipleSelection(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetMultipleSelection, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Whether typing can be performed into multiple selections
	 */
	bool Editor::AdditionalSelectionTyping()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetAdditionalSelectionTyping, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set whether typing can be performed into multiple selections
	 */
	void Editor::AdditionalSelectionTyping(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetAdditionalSelectionTyping, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Whether additional carets will blink
	 */
	bool Editor::AdditionalCaretsBlink()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetAdditionalCaretsBlink, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set whether additional carets will blink
	 */
	void Editor::AdditionalCaretsBlink(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetAdditionalCaretsBlink, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Whether additional carets are visible
	 */
	bool Editor::AdditionalCaretsVisible()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetAdditionalCaretsVisible, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set whether additional carets are visible
	 */
	void Editor::AdditionalCaretsVisible(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetAdditionalCaretsVisible, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * How many selections are there?
	 */
	int32_t Editor::Selections()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetSelections, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Is every selected range empty?
	 */
	bool Editor::SelectionEmpty()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetSelectionEmpty, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Which selection is the main selection
	 */
	int32_t Editor::MainSelection()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetMainSelection, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the main selection
	 */
	void Editor::MainSelection(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetMainSelection, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Return the caret position of the rectangular selection.
	 */
	int64_t Editor::RectangularSelectionCaret()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetRectangularSelectionCaret, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the caret position of the rectangular selection.
	 */
	void Editor::RectangularSelectionCaret(int64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetRectangularSelectionCaret, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Return the anchor position of the rectangular selection.
	 */
	int64_t Editor::RectangularSelectionAnchor()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetRectangularSelectionAnchor, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the anchor position of the rectangular selection.
	 */
	void Editor::RectangularSelectionAnchor(int64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetRectangularSelectionAnchor, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Return the virtual space of the caret of the rectangular selection.
	 */
	int64_t Editor::RectangularSelectionCaretVirtualSpace()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetRectangularSelectionCaretVirtualSpace, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the virtual space of the caret of the rectangular selection.
	 */
	void Editor::RectangularSelectionCaretVirtualSpace(int64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetRectangularSelectionCaretVirtualSpace, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Return the virtual space of the anchor of the rectangular selection.
	 */
	int64_t Editor::RectangularSelectionAnchorVirtualSpace()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetRectangularSelectionAnchorVirtualSpace, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the virtual space of the anchor of the rectangular selection.
	 */
	void Editor::RectangularSelectionAnchorVirtualSpace(int64_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetRectangularSelectionAnchorVirtualSpace, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Return options for virtual space behaviour.
	 */
	MicaEditor::VirtualSpace Editor::VirtualSpaceOptions()
	{
		return static_cast<MicaEditor::VirtualSpace>(_scintilla->WndProc(Scintilla::Message::GetVirtualSpaceOptions, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set options for virtual space behaviour.
	 */
	void Editor::VirtualSpaceOptions(MicaEditor::VirtualSpace const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetVirtualSpaceOptions, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the modifier key used for rectangular selection.
	 */
	int32_t Editor::RectangularSelectionModifier()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetRectangularSelectionModifier, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	void Editor::RectangularSelectionModifier(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetRectangularSelectionModifier, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the alpha of the selection.
	 */
	MicaEditor::Alpha Editor::AdditionalSelAlpha()
	{
		return static_cast<MicaEditor::Alpha>(_scintilla->WndProc(Scintilla::Message::GetAdditionalSelAlpha, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the alpha of the selection.
	 */
	void Editor::AdditionalSelAlpha(MicaEditor::Alpha const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetAdditionalSelAlpha, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the foreground colour of additional carets.
	 */
	int32_t Editor::AdditionalCaretFore()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetAdditionalCaretFore, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the foreground colour of additional carets.
	 */
	void Editor::AdditionalCaretFore(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetAdditionalCaretFore, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the identifier.
	 */
	int32_t Editor::Identifier()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetIdentifier, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the identifier reported as idFrom in notification messages.
	 */
	void Editor::Identifier(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::SetIdentifier, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the tech.
	 */
	MicaEditor::Technology Editor::Technology()
	{
		return static_cast<MicaEditor::Technology>(_scintilla->WndProc(Scintilla::Message::GetTechnology, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the technology used.
	 */
	void Editor::Technology(MicaEditor::Technology const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetTechnology, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Is the caret line always visible?
	 */
	bool Editor::CaretLineVisibleAlways()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetCaretLineVisibleAlways, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Sets the caret line to always visible.
	 */
	void Editor::CaretLineVisibleAlways(bool value)
	{
		_scintilla->WndProc(Scintilla::Message::SetCaretLineVisibleAlways, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the line end types currently allowed.
	 */
	MicaEditor::LineEndType Editor::LineEndTypesAllowed()
	{
		return static_cast<MicaEditor::LineEndType>(_scintilla->WndProc(Scintilla::Message::GetLineEndTypesAllowed, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the line end types that the application wants to use. May not be used if incompatible with lexer or encoding.
	 */
	void Editor::LineEndTypesAllowed(MicaEditor::LineEndType const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetLineEndTypesAllowed, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the line end types currently recognised. May be a subset of the allowed types due to lexer limitation.
	 */
	MicaEditor::LineEndType Editor::LineEndTypesActive()
	{
		return static_cast<MicaEditor::LineEndType>(_scintilla->WndProc(Scintilla::Message::GetLineEndTypesActive, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the visibility for the end of line annotations for a view
	 */
	MicaEditor::EOLAnnotationVisible Editor::EOLAnnotationVisible()
	{
		return static_cast<MicaEditor::EOLAnnotationVisible>(_scintilla->WndProc(Scintilla::Message::EOLAnnotationGetVisible, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set the visibility for the end of line annotations for a view
	 */
	void Editor::EOLAnnotationVisible(MicaEditor::EOLAnnotationVisible const &value)
	{
		_scintilla->WndProc(Scintilla::Message::EOLAnnotationSetVisible, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get the start of the range of style numbers used for end of line annotations
	 */
	int32_t Editor::EOLAnnotationStyleOffset()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::EOLAnnotationGetStyleOffset, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the start of the range of style numbers used for end of line annotations
	 */
	void Editor::EOLAnnotationStyleOffset(int32_t value)
	{
		_scintilla->WndProc(Scintilla::Message::EOLAnnotationSetStyleOffset, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve line character index state.
	 */
	MicaEditor::LineCharacterIndexType Editor::LineCharacterIndex()
	{
		return static_cast<MicaEditor::LineCharacterIndexType>(_scintilla->WndProc(Scintilla::Message::GetLineCharacterIndex, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the lexing language of the document.
	 */
	int32_t Editor::Lexer()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetLexer, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Bit set of LineEndType enumertion for which line ends beyond the standard
	 * LF, CR, and CRLF are supported by the lexer.
	 */
	MicaEditor::LineEndType Editor::LineEndTypesSupported()
	{
		return static_cast<MicaEditor::LineEndType>(_scintilla->WndProc(Scintilla::Message::GetLineEndTypesSupported, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Where styles are duplicated by a feature such as active/inactive code
	 * return the distance between the two types.
	 */
	int32_t Editor::DistanceToSecondaryStyles()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::DistanceToSecondaryStyles, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the number of named styles for the lexer.
	 */
	int32_t Editor::NamedStyles()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetNamedStyles, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve bidirectional text display state.
	 */
	MicaEditor::Bidirectional Editor::Bidirectional()
	{
		return static_cast<MicaEditor::Bidirectional>(_scintilla->WndProc(Scintilla::Message::GetBidirectional, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Set bidirectional text display state.
	 */
	void Editor::Bidirectional(MicaEditor::Bidirectional const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetBidirectional, static_cast<Scintilla::uptr_t>(value), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Add text to the document at current position.
	 */
	void Editor::AddTextFromBuffer(int64_t length, Windows::Storage::Streams::IBuffer const &text)
	{
		_scintilla->WndProc(Scintilla::Message::AddText, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr));
	}

	void Editor::AddText(int64_t length, hstring const &text)
	{
		_scintilla->WndProc(Scintilla::Message::AddText, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str()));
	}

	/*
	 * Add array of cells to document.
	 */
	void Editor::AddStyledText(int64_t length, array_view<uint8_t const> c)
	{
		_scintilla->WndProc(Scintilla::Message::AddStyledText, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(c.data()));
	}

	/*
	 * Insert string at a position.
	 */
	void Editor::InsertTextFromBuffer(int64_t pos, Windows::Storage::Streams::IBuffer const &text)
	{
		_scintilla->WndProc(Scintilla::Message::InsertText, static_cast<Scintilla::uptr_t>(pos), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr));
	}

	void Editor::InsertText(int64_t pos, hstring const &text)
	{
		_scintilla->WndProc(Scintilla::Message::InsertText, static_cast<Scintilla::uptr_t>(pos), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str()));
	}

	/*
	 * Change the text that is being inserted in response to SC_MOD_INSERTCHECK
	 */
	void Editor::ChangeInsertionFromBuffer(int64_t length, Windows::Storage::Streams::IBuffer const &text)
	{
		_scintilla->WndProc(Scintilla::Message::ChangeInsertion, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr));
	}

	void Editor::ChangeInsertion(int64_t length, hstring const &text)
	{
		_scintilla->WndProc(Scintilla::Message::ChangeInsertion, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str()));
	}

	/*
	 * Delete all text in the document.
	 */
	void Editor::ClearAll()
	{
		_scintilla->WndProc(Scintilla::Message::ClearAll, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Delete a range of text in the document.
	 */
	void Editor::DeleteRange(int64_t start, int64_t lengthDelete)
	{
		_scintilla->WndProc(Scintilla::Message::DeleteRange, static_cast<Scintilla::uptr_t>(start), static_cast<Scintilla::sptr_t>(lengthDelete));
	}

	/*
	 * Set all style bytes to 0, remove all folding information.
	 */
	void Editor::ClearDocumentStyle()
	{
		_scintilla->WndProc(Scintilla::Message::ClearDocumentStyle, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Redoes the next action on the undo history.
	 */
	void Editor::Redo()
	{
		_scintilla->WndProc(Scintilla::Message::Redo, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Select all the text in the document.
	 */
	void Editor::SelectAll()
	{
		_scintilla->WndProc(Scintilla::Message::SelectAll, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Remember the current position in the undo history as the position
	 * at which the document was saved.
	 */
	void Editor::SetSavePoint()
	{
		_scintilla->WndProc(Scintilla::Message::SetSavePoint, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve a buffer of cells.
	 * Returns the number of bytes in the buffer not including terminating NULs.
	 */
	int64_t Editor::GetStyledText(uint64_t tr)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetStyledText, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(tr)));
	}

	/*
	 * Retrieve a buffer of cells that can be past 2GB.
	 * Returns the number of bytes in the buffer not including terminating NULs.
	 */
	int64_t Editor::GetStyledTextFull(uint64_t tr)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetStyledTextFull, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(tr)));
	}

	/*
	 * Are there any redoable actions in the undo history?
	 */
	bool Editor::CanRedo()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::CanRedo, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the line number at which a particular marker is located.
	 */
	int64_t Editor::MarkerLineFromHandle(int32_t markerHandle)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::MarkerLineFromHandle, static_cast<Scintilla::uptr_t>(markerHandle), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Delete a marker.
	 */
	void Editor::MarkerDeleteHandle(int32_t markerHandle)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerDeleteHandle, static_cast<Scintilla::uptr_t>(markerHandle), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve marker handles of a line
	 */
	int32_t Editor::MarkerHandleFromLine(int64_t line, int32_t which)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::MarkerHandleFromLine, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(which)));
	}

	/*
	 * Retrieve marker number of a marker handle
	 */
	int32_t Editor::MarkerNumberFromLine(int64_t line, int32_t which)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::MarkerNumberFromLine, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(which)));
	}

	/*
	 * Find the position from a point within the window.
	 */
	int64_t Editor::PositionFromPoint(int32_t x, int32_t y)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::PositionFromPoint, static_cast<Scintilla::uptr_t>(x), static_cast<Scintilla::sptr_t>(y)));
	}

	/*
	 * Find the position from a point within the window but return
	 * INVALID_POSITION if not close to text.
	 */
	int64_t Editor::PositionFromPointClose(int32_t x, int32_t y)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::PositionFromPointClose, static_cast<Scintilla::uptr_t>(x), static_cast<Scintilla::sptr_t>(y)));
	}

	/*
	 * Set caret to start of a line and ensure it is visible.
	 */
	void Editor::GotoLine(int64_t line)
	{
		_scintilla->WndProc(Scintilla::Message::GotoLine, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set caret to a position and ensure it is visible.
	 */
	void Editor::GotoPos(int64_t caret)
	{
		_scintilla->WndProc(Scintilla::Message::GotoPos, static_cast<Scintilla::uptr_t>(caret), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the text of the line containing the caret.
	 * Returns the index of the caret on the line.
	 * Result is NUL-terminated.
	 */
	int64_t Editor::GetCurLineWriteBuffer(int64_t length, Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetCurLine, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	hstring Editor::GetCurLine(int64_t length)
	{
		const auto wParam{ static_cast<Scintilla::uptr_t>(length) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetCurLine, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetCurLine, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Convert all line endings in the document to one mode.
	 */
	void Editor::ConvertEOLs(MicaEditor::EndOfLine const &eolMode)
	{
		_scintilla->WndProc(Scintilla::Message::ConvertEOLs, static_cast<Scintilla::uptr_t>(eolMode), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the current styling position to start.
	 * The unused parameter is no longer used and should be set to 0.
	 */
	void Editor::StartStyling(int64_t start, int32_t unused)
	{
		_scintilla->WndProc(Scintilla::Message::StartStyling, static_cast<Scintilla::uptr_t>(start), static_cast<Scintilla::sptr_t>(unused));
	}

	/*
	 * Change style from current styling position for length characters to a style
	 * and move the current styling position to after this newly styled segment.
	 */
	void Editor::SetStyling(int64_t length, int32_t style)
	{
		_scintilla->WndProc(Scintilla::Message::SetStyling, static_cast<Scintilla::uptr_t>(length), static_cast<Scintilla::sptr_t>(style));
	}

	/*
	 * Clear explicit tabstops on a line.
	 */
	void Editor::ClearTabStops(int64_t line)
	{
		_scintilla->WndProc(Scintilla::Message::ClearTabStops, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Add an explicit tab stop for a line.
	 */
	void Editor::AddTabStop(int64_t line, int32_t x)
	{
		_scintilla->WndProc(Scintilla::Message::AddTabStop, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(x));
	}

	/*
	 * Find the next explicit tab stop position on a line after a position.
	 */
	int32_t Editor::GetNextTabStop(int64_t line, int32_t x)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetNextTabStop, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(x)));
	}

	/*
	 * Set the symbol used for a particular marker number.
	 */
	void Editor::MarkerDefine(int32_t markerNumber, MicaEditor::MarkerSymbol const &markerSymbol)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerDefine, static_cast<Scintilla::uptr_t>(markerNumber), static_cast<Scintilla::sptr_t>(markerSymbol));
	}

	/*
	 * Enable/disable highlight for current folding block (smallest one that contains the caret)
	 */
	void Editor::MarkerEnableHighlight(bool enabled)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerEnableHighlight, static_cast<Scintilla::uptr_t>(enabled), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Add a marker to a line, returning an ID which can be used to find or delete the marker.
	 */
	int32_t Editor::MarkerAdd(int64_t line, int32_t markerNumber)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::MarkerAdd, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(markerNumber)));
	}

	/*
	 * Delete a marker from a line.
	 */
	void Editor::MarkerDelete(int64_t line, int32_t markerNumber)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerDelete, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(markerNumber));
	}

	/*
	 * Delete all markers with a particular number from all lines.
	 */
	void Editor::MarkerDeleteAll(int32_t markerNumber)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerDeleteAll, static_cast<Scintilla::uptr_t>(markerNumber), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get a bit mask of all the markers set on a line.
	 */
	int32_t Editor::MarkerGet(int64_t line)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::MarkerGet, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Find the next line at or after lineStart that includes a marker in mask.
	 * Return -1 when no more lines.
	 */
	int64_t Editor::MarkerNext(int64_t lineStart, int32_t markerMask)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::MarkerNext, static_cast<Scintilla::uptr_t>(lineStart), static_cast<Scintilla::sptr_t>(markerMask)));
	}

	/*
	 * Find the previous line before lineStart that includes a marker in mask.
	 */
	int64_t Editor::MarkerPrevious(int64_t lineStart, int32_t markerMask)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::MarkerPrevious, static_cast<Scintilla::uptr_t>(lineStart), static_cast<Scintilla::sptr_t>(markerMask)));
	}

	/*
	 * Define a marker from a pixmap.
	 */
	void Editor::MarkerDefinePixmapFromBuffer(int32_t markerNumber, Windows::Storage::Streams::IBuffer const &pixmap)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerDefinePixmap, static_cast<Scintilla::uptr_t>(markerNumber), reinterpret_cast<Scintilla::sptr_t>(pixmap ? pixmap.data() : nullptr));
	}

	void Editor::MarkerDefinePixmap(int32_t markerNumber, hstring const &pixmap)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerDefinePixmap, static_cast<Scintilla::uptr_t>(markerNumber), reinterpret_cast<Scintilla::sptr_t>(to_string(pixmap).c_str()));
	}

	/*
	 * Add a set of markers to a line.
	 */
	void Editor::MarkerAddSet(int64_t line, int32_t markerSet)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerAddSet, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(markerSet));
	}

	/*
	 * Clear all the styles and make equivalent to the global default style.
	 */
	void Editor::StyleClearAll()
	{
		_scintilla->WndProc(Scintilla::Message::StyleClearAll, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Reset the default style to its state at startup
	 */
	void Editor::StyleResetDefault()
	{
		_scintilla->WndProc(Scintilla::Message::StyleResetDefault, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Use the default or platform-defined colour for an element.
	 */
	void Editor::ResetElementColour(MicaEditor::Element const &element)
	{
		_scintilla->WndProc(Scintilla::Message::ResetElementColour, static_cast<Scintilla::uptr_t>(element), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the foreground colour of the main and additional selections and whether to use this setting.
	 */
	void Editor::SetSelFore(bool useSetting, int32_t fore)
	{
		_scintilla->WndProc(Scintilla::Message::SetSelFore, static_cast<Scintilla::uptr_t>(useSetting), static_cast<Scintilla::sptr_t>(fore));
	}

	/*
	 * Set the background colour of the main and additional selections and whether to use this setting.
	 */
	void Editor::SetSelBack(bool useSetting, int32_t back)
	{
		_scintilla->WndProc(Scintilla::Message::SetSelBack, static_cast<Scintilla::uptr_t>(useSetting), static_cast<Scintilla::sptr_t>(back));
	}

	/*
	 * When key+modifier combination keyDefinition is pressed perform sciCommand.
	 */
	void Editor::AssignCmdKey(int32_t keyDefinition, int32_t sciCommand)
	{
		_scintilla->WndProc(Scintilla::Message::AssignCmdKey, static_cast<Scintilla::uptr_t>(keyDefinition), static_cast<Scintilla::sptr_t>(sciCommand));
	}

	/*
	 * When key+modifier combination keyDefinition is pressed do nothing.
	 */
	void Editor::ClearCmdKey(int32_t keyDefinition)
	{
		_scintilla->WndProc(Scintilla::Message::ClearCmdKey, static_cast<Scintilla::uptr_t>(keyDefinition), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Drop all key mappings.
	 */
	void Editor::ClearAllCmdKeys()
	{
		_scintilla->WndProc(Scintilla::Message::ClearAllCmdKeys, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the styles for a segment of the document.
	 */
	void Editor::SetStylingExFromBuffer(int64_t length, Windows::Storage::Streams::IBuffer const &styles)
	{
		_scintilla->WndProc(Scintilla::Message::SetStylingEx, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(styles ? styles.data() : nullptr));
	}

	void Editor::SetStylingEx(int64_t length, hstring const &styles)
	{
		_scintilla->WndProc(Scintilla::Message::SetStylingEx, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(to_string(styles).c_str()));
	}

	/*
	 * Start a sequence of actions that is undone and redone as a unit.
	 * May be nested.
	 */
	void Editor::BeginUndoAction()
	{
		_scintilla->WndProc(Scintilla::Message::BeginUndoAction, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * End a sequence of actions that is undone and redone as a unit.
	 */
	void Editor::EndUndoAction()
	{
		_scintilla->WndProc(Scintilla::Message::EndUndoAction, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the foreground colour of all whitespace and whether to use this setting.
	 */
	void Editor::SetWhitespaceFore(bool useSetting, int32_t fore)
	{
		_scintilla->WndProc(Scintilla::Message::SetWhitespaceFore, static_cast<Scintilla::uptr_t>(useSetting), static_cast<Scintilla::sptr_t>(fore));
	}

	/*
	 * Set the background colour of all whitespace and whether to use this setting.
	 */
	void Editor::SetWhitespaceBack(bool useSetting, int32_t back)
	{
		_scintilla->WndProc(Scintilla::Message::SetWhitespaceBack, static_cast<Scintilla::uptr_t>(useSetting), static_cast<Scintilla::sptr_t>(back));
	}

	/*
	 * Display a auto-completion list.
	 * The lengthEntered parameter indicates how many characters before
	 * the caret should be used to provide context.
	 */
	void Editor::AutoCShowFromBuffer(int64_t lengthEntered, Windows::Storage::Streams::IBuffer const &itemList)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCShow, static_cast<Scintilla::uptr_t>(lengthEntered), reinterpret_cast<Scintilla::sptr_t>(itemList ? itemList.data() : nullptr));
	}

	void Editor::AutoCShow(int64_t lengthEntered, hstring const &itemList)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCShow, static_cast<Scintilla::uptr_t>(lengthEntered), reinterpret_cast<Scintilla::sptr_t>(to_string(itemList).c_str()));
	}

	/*
	 * Remove the auto-completion list from the screen.
	 */
	void Editor::AutoCCancel()
	{
		_scintilla->WndProc(Scintilla::Message::AutoCCancel, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Is there an auto-completion list visible?
	 */
	bool Editor::AutoCActive()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::AutoCActive, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the position of the caret when the auto-completion list was displayed.
	 */
	int64_t Editor::AutoCPosStart()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::AutoCPosStart, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * User has selected an item so remove the list and insert the selection.
	 */
	void Editor::AutoCComplete()
	{
		_scintilla->WndProc(Scintilla::Message::AutoCComplete, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Define a set of character that when typed cancel the auto-completion list.
	 */
	void Editor::AutoCStopsFromBuffer(Windows::Storage::Streams::IBuffer const &characterSet)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCStops, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(characterSet ? characterSet.data() : nullptr));
	}

	void Editor::AutoCStops(hstring const &characterSet)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCStops, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(to_string(characterSet).c_str()));
	}

	/*
	 * Select the item in the auto-completion list that starts with a string.
	 */
	void Editor::AutoCSelectFromBuffer(Windows::Storage::Streams::IBuffer const &select)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSelect, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(select ? select.data() : nullptr));
	}

	void Editor::AutoCSelect(hstring const &select)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSelect, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(to_string(select).c_str()));
	}

	/*
	 * Display a list of strings and send notification when user chooses one.
	 */
	void Editor::UserListShowFromBuffer(int32_t listType, Windows::Storage::Streams::IBuffer const &itemList)
	{
		_scintilla->WndProc(Scintilla::Message::UserListShow, static_cast<Scintilla::uptr_t>(listType), reinterpret_cast<Scintilla::sptr_t>(itemList ? itemList.data() : nullptr));
	}

	void Editor::UserListShow(int32_t listType, hstring const &itemList)
	{
		_scintilla->WndProc(Scintilla::Message::UserListShow, static_cast<Scintilla::uptr_t>(listType), reinterpret_cast<Scintilla::sptr_t>(to_string(itemList).c_str()));
	}

	/*
	 * Register an XPM image for use in autocompletion lists.
	 */
	void Editor::RegisterImageFromBuffer(int32_t type, Windows::Storage::Streams::IBuffer const &xpmData)
	{
		_scintilla->WndProc(Scintilla::Message::RegisterImage, static_cast<Scintilla::uptr_t>(type), reinterpret_cast<Scintilla::sptr_t>(xpmData ? xpmData.data() : nullptr));
	}

	void Editor::RegisterImage(int32_t type, hstring const &xpmData)
	{
		_scintilla->WndProc(Scintilla::Message::RegisterImage, static_cast<Scintilla::uptr_t>(type), reinterpret_cast<Scintilla::sptr_t>(to_string(xpmData).c_str()));
	}

	/*
	 * Clear all the registered XPM images.
	 */
	void Editor::ClearRegisteredImages()
	{
		_scintilla->WndProc(Scintilla::Message::ClearRegisteredImages, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Count characters between two positions.
	 */
	int64_t Editor::CountCharacters(int64_t start, int64_t end)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::CountCharacters, static_cast<Scintilla::uptr_t>(start), static_cast<Scintilla::sptr_t>(end)));
	}

	/*
	 * Count code units between two positions.
	 */
	int64_t Editor::CountCodeUnits(int64_t start, int64_t end)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::CountCodeUnits, static_cast<Scintilla::uptr_t>(start), static_cast<Scintilla::sptr_t>(end)));
	}

	/*
	 * Set caret to a position, while removing any existing selection.
	 */
	void Editor::SetEmptySelection(int64_t caret)
	{
		_scintilla->WndProc(Scintilla::Message::SetEmptySelection, static_cast<Scintilla::uptr_t>(caret), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Find some text in the document.
	 */
	int64_t Editor::FindText(MicaEditor::FindOption const &searchFlags, uint64_t ft)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::FindText, static_cast<Scintilla::uptr_t>(searchFlags), static_cast<Scintilla::sptr_t>(ft)));
	}

	/*
	 * Find some text in the document.
	 */
	int64_t Editor::FindTextFull(MicaEditor::FindOption const &searchFlags, uint64_t ft)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::FindTextFull, static_cast<Scintilla::uptr_t>(searchFlags), static_cast<Scintilla::sptr_t>(ft)));
	}

	/*
	 * Draw the document into a display context such as a printer.
	 */
	int64_t Editor::FormatRange(bool draw, uint64_t fr)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::FormatRange, static_cast<Scintilla::uptr_t>(draw), static_cast<Scintilla::sptr_t>(fr)));
	}

	/*
	 * Draw the document into a display context such as a printer.
	 */
	int64_t Editor::FormatRangeFull(bool draw, uint64_t fr)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::FormatRangeFull, static_cast<Scintilla::uptr_t>(draw), static_cast<Scintilla::sptr_t>(fr)));
	}

	/*
	 * Retrieve the contents of a line.
	 * Returns the length of the line.
	 */
	int64_t Editor::GetLineWriteBuffer(int64_t line, Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetLine, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	hstring Editor::GetLine(int64_t line)
	{
		const auto wParam{ static_cast<Scintilla::uptr_t>(line) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetLine, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetLine, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Select a range of text.
	 */
	void Editor::SetSel(int64_t anchor, int64_t caret)
	{
		_scintilla->WndProc(Scintilla::Message::SetSel, static_cast<Scintilla::uptr_t>(anchor), static_cast<Scintilla::sptr_t>(caret));
	}

	/*
	 * Retrieve the selected text.
	 * Return the length of the text.
	 * Result is NUL-terminated.
	 */
	int64_t Editor::GetSelTextWriteBuffer(Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetSelText, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	hstring Editor::GetSelText()
	{
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetSelText, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetSelText, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Retrieve a range of text.
	 * Return the length of the text.
	 */
	int64_t Editor::GetTextRange(uint64_t tr)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetTextRange, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(tr)));
	}

	/*
	 * Retrieve a range of text that can be past 2GB.
	 * Return the length of the text.
	 */
	int64_t Editor::GetTextRangeFull(uint64_t tr)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetTextRangeFull, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(tr)));
	}

	/*
	 * Draw the selection either highlighted or in normal (non-highlighted) style.
	 */
	void Editor::HideSelection(bool hide)
	{
		_scintilla->WndProc(Scintilla::Message::HideSelection, static_cast<Scintilla::uptr_t>(hide), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the x value of the point in the window where a position is displayed.
	 */
	int32_t Editor::PointXFromPosition(int64_t pos)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::PointXFromPosition, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(pos)));
	}

	/*
	 * Retrieve the y value of the point in the window where a position is displayed.
	 */
	int32_t Editor::PointYFromPosition(int64_t pos)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::PointYFromPosition, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(pos)));
	}

	/*
	 * Retrieve the line containing a position.
	 */
	int64_t Editor::LineFromPosition(int64_t pos)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::LineFromPosition, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the position at the start of a line.
	 */
	int64_t Editor::PositionFromLine(int64_t line)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::PositionFromLine, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Scroll horizontally and vertically.
	 */
	void Editor::LineScroll(int64_t columns, int64_t lines)
	{
		_scintilla->WndProc(Scintilla::Message::LineScroll, static_cast<Scintilla::uptr_t>(columns), static_cast<Scintilla::sptr_t>(lines));
	}

	/*
	 * Ensure the caret is visible.
	 */
	void Editor::ScrollCaret()
	{
		_scintilla->WndProc(Scintilla::Message::ScrollCaret, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Scroll the argument positions and the range between them into view giving
	 * priority to the primary position then the secondary position.
	 * This may be used to make a search match visible.
	 */
	void Editor::ScrollRange(int64_t secondary, int64_t primary)
	{
		_scintilla->WndProc(Scintilla::Message::ScrollRange, static_cast<Scintilla::uptr_t>(secondary), static_cast<Scintilla::sptr_t>(primary));
	}

	/*
	 * Replace the selected text with the argument text.
	 */
	void Editor::ReplaceSelFromBuffer(Windows::Storage::Streams::IBuffer const &text)
	{
		_scintilla->WndProc(Scintilla::Message::ReplaceSel, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr));
	}

	void Editor::ReplaceSel(hstring const &text)
	{
		_scintilla->WndProc(Scintilla::Message::ReplaceSel, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str()));
	}

	/*
	 * Null operation.
	 */
	void Editor::Null()
	{
		_scintilla->WndProc(Scintilla::Message::Null, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Will a paste succeed?
	 */
	bool Editor::CanPaste()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::CanPaste, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Are there any undoable actions in the undo history?
	 */
	bool Editor::CanUndo()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::CanUndo, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Delete the undo history.
	 */
	void Editor::EmptyUndoBuffer()
	{
		_scintilla->WndProc(Scintilla::Message::EmptyUndoBuffer, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Undo one action in the undo history.
	 */
	void Editor::Undo()
	{
		_scintilla->WndProc(Scintilla::Message::Undo, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Cut the selection to the clipboard.
	 */
	void Editor::Cut()
	{
		_scintilla->WndProc(Scintilla::Message::Cut, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Copy the selection to the clipboard.
	 */
	void Editor::Copy()
	{
		_scintilla->WndProc(Scintilla::Message::Copy, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Paste the contents of the clipboard into the document replacing the selection.
	 */
	void Editor::Paste()
	{
		_scintilla->WndProc(Scintilla::Message::Paste, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Clear the selection.
	 */
	void Editor::Clear()
	{
		_scintilla->WndProc(Scintilla::Message::Clear, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Replace the contents of the document with the argument text.
	 */
	void Editor::SetTextFromBuffer(Windows::Storage::Streams::IBuffer const &text)
	{
		_scintilla->WndProc(Scintilla::Message::SetText, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr));
	}

	void Editor::SetText(hstring const &text)
	{
		_scintilla->WndProc(Scintilla::Message::SetText, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str()));
	}

	/*
	 * Retrieve all the text in the document.
	 * Returns number of characters retrieved.
	 * Result is NUL-terminated.
	 */
	int64_t Editor::GetTextWriteBuffer(int64_t length, Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetText, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	hstring Editor::GetText(int64_t length)
	{
		const auto wParam{ static_cast<Scintilla::uptr_t>(length) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetText, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetText, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Sets both the start and end of the target in one call.
	 */
	void Editor::SetTargetRange(int64_t start, int64_t end)
	{
		_scintilla->WndProc(Scintilla::Message::SetTargetRange, static_cast<Scintilla::uptr_t>(start), static_cast<Scintilla::sptr_t>(end));
	}

	/*
	 * Make the target range start and end be the same as the selection range start and end.
	 */
	void Editor::TargetFromSelection()
	{
		_scintilla->WndProc(Scintilla::Message::TargetFromSelection, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Sets the target to the whole document.
	 */
	void Editor::TargetWholeDocument()
	{
		_scintilla->WndProc(Scintilla::Message::TargetWholeDocument, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Replace the target text with the argument text.
	 * Text is counted so it can contain NULs.
	 * Returns the length of the replacement text.
	 */
	int64_t Editor::ReplaceTargetFromBuffer(int64_t length, Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::ReplaceTarget, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	int64_t Editor::ReplaceTarget(int64_t length, hstring const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::ReplaceTarget, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str())));
	}

	/*
	 * Replace the target text with the argument text after \d processing.
	 * Text is counted so it can contain NULs.
	 * Looks for \d where d is between 1 and 9 and replaces these with the strings
	 * matched in the last search operation which were surrounded by \( and \).
	 * Returns the length of the replacement text including any change
	 * caused by processing the \d patterns.
	 */
	int64_t Editor::ReplaceTargetREFromBuffer(int64_t length, Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::ReplaceTargetRE, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	int64_t Editor::ReplaceTargetRE(int64_t length, hstring const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::ReplaceTargetRE, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str())));
	}

	/*
	 * Replace the target text with the argument text but ignore prefix and suffix that
	 * are the same as current.
	 */
	int64_t Editor::ReplaceTargetMinimalFromBuffer(int64_t length, Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::ReplaceTargetMinimal, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	int64_t Editor::ReplaceTargetMinimal(int64_t length, hstring const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::ReplaceTargetMinimal, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str())));
	}

	/*
	 * Search for a counted string in the target and set the target to the found
	 * range. Text is counted so it can contain NULs.
	 * Returns start of found range or -1 for failure in which case target is not moved.
	 */
	int64_t Editor::SearchInTargetFromBuffer(int64_t length, Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::SearchInTarget, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	int64_t Editor::SearchInTarget(int64_t length, hstring const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::SearchInTarget, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str())));
	}

	/*
	 * Show a call tip containing a definition near position pos.
	 */
	void Editor::CallTipShowFromBuffer(int64_t pos, Windows::Storage::Streams::IBuffer const &definition)
	{
		_scintilla->WndProc(Scintilla::Message::CallTipShow, static_cast<Scintilla::uptr_t>(pos), reinterpret_cast<Scintilla::sptr_t>(definition ? definition.data() : nullptr));
	}

	void Editor::CallTipShow(int64_t pos, hstring const &definition)
	{
		_scintilla->WndProc(Scintilla::Message::CallTipShow, static_cast<Scintilla::uptr_t>(pos), reinterpret_cast<Scintilla::sptr_t>(to_string(definition).c_str()));
	}

	/*
	 * Remove the call tip from the screen.
	 */
	void Editor::CallTipCancel()
	{
		_scintilla->WndProc(Scintilla::Message::CallTipCancel, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Is there an active call tip?
	 */
	bool Editor::CallTipActive()
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::CallTipActive, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the position where the caret was before displaying the call tip.
	 */
	int64_t Editor::CallTipPosStart()
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::CallTipPosStart, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Highlight a segment of the definition.
	 */
	void Editor::CallTipSetHlt(int64_t highlightStart, int64_t highlightEnd)
	{
		_scintilla->WndProc(Scintilla::Message::CallTipSetHlt, static_cast<Scintilla::uptr_t>(highlightStart), static_cast<Scintilla::sptr_t>(highlightEnd));
	}

	/*
	 * Find the display line of a document line taking hidden lines into account.
	 */
	int64_t Editor::VisibleFromDocLine(int64_t docLine)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::VisibleFromDocLine, static_cast<Scintilla::uptr_t>(docLine), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Find the document line of a display line taking hidden lines into account.
	 */
	int64_t Editor::DocLineFromVisible(int64_t displayLine)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::DocLineFromVisible, static_cast<Scintilla::uptr_t>(displayLine), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * The number of display lines needed to wrap a document line
	 */
	int64_t Editor::WrapCount(int64_t docLine)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::WrapCount, static_cast<Scintilla::uptr_t>(docLine), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Make a range of lines visible.
	 */
	void Editor::ShowLines(int64_t lineStart, int64_t lineEnd)
	{
		_scintilla->WndProc(Scintilla::Message::ShowLines, static_cast<Scintilla::uptr_t>(lineStart), static_cast<Scintilla::sptr_t>(lineEnd));
	}

	/*
	 * Make a range of lines invisible.
	 */
	void Editor::HideLines(int64_t lineStart, int64_t lineEnd)
	{
		_scintilla->WndProc(Scintilla::Message::HideLines, static_cast<Scintilla::uptr_t>(lineStart), static_cast<Scintilla::sptr_t>(lineEnd));
	}

	/*
	 * Switch a header line between expanded and contracted.
	 */
	void Editor::ToggleFold(int64_t line)
	{
		_scintilla->WndProc(Scintilla::Message::ToggleFold, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Switch a header line between expanded and contracted and show some text after the line.
	 */
	void Editor::ToggleFoldShowTextFromBuffer(int64_t line, Windows::Storage::Streams::IBuffer const &text)
	{
		_scintilla->WndProc(Scintilla::Message::ToggleFoldShowText, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr));
	}

	void Editor::ToggleFoldShowText(int64_t line, hstring const &text)
	{
		_scintilla->WndProc(Scintilla::Message::ToggleFoldShowText, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str()));
	}

	/*
	 * Set the default fold display text.
	 */
	void Editor::SetDefaultFoldDisplayTextFromBuffer(Windows::Storage::Streams::IBuffer const &text)
	{
		_scintilla->WndProc(Scintilla::Message::SetDefaultFoldDisplayText, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr));
	}

	void Editor::SetDefaultFoldDisplayText(hstring const &text)
	{
		_scintilla->WndProc(Scintilla::Message::SetDefaultFoldDisplayText, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str()));
	}

	/*
	 * Get the default fold display text.
	 */
	int32_t Editor::GetDefaultFoldDisplayTextWriteBuffer(Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetDefaultFoldDisplayText, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	hstring Editor::GetDefaultFoldDisplayText()
	{
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetDefaultFoldDisplayText, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetDefaultFoldDisplayText, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Expand or contract a fold header.
	 */
	void Editor::FoldLine(int64_t line, MicaEditor::FoldAction const &action)
	{
		_scintilla->WndProc(Scintilla::Message::FoldLine, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(action));
	}

	/*
	 * Expand or contract a fold header and its children.
	 */
	void Editor::FoldChildren(int64_t line, MicaEditor::FoldAction const &action)
	{
		_scintilla->WndProc(Scintilla::Message::FoldChildren, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(action));
	}

	/*
	 * Expand a fold header and all children. Use the level argument instead of the line's current level.
	 */
	void Editor::ExpandChildren(int64_t line, MicaEditor::FoldLevel const &level)
	{
		_scintilla->WndProc(Scintilla::Message::ExpandChildren, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(level));
	}

	/*
	 * Expand or contract all fold headers.
	 */
	void Editor::FoldAll(MicaEditor::FoldAction const &action)
	{
		_scintilla->WndProc(Scintilla::Message::FoldAll, static_cast<Scintilla::uptr_t>(action), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Ensure a particular line is visible by expanding any header line hiding it.
	 */
	void Editor::EnsureVisible(int64_t line)
	{
		_scintilla->WndProc(Scintilla::Message::EnsureVisible, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Ensure a particular line is visible by expanding any header line hiding it.
	 * Use the currently set visibility policy to determine which range to display.
	 */
	void Editor::EnsureVisibleEnforcePolicy(int64_t line)
	{
		_scintilla->WndProc(Scintilla::Message::EnsureVisibleEnforcePolicy, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Get position of start of word.
	 */
	int64_t Editor::WordStartPosition(int64_t pos, bool onlyWordCharacters)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::WordStartPosition, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(onlyWordCharacters)));
	}

	/*
	 * Get position of end of word.
	 */
	int64_t Editor::WordEndPosition(int64_t pos, bool onlyWordCharacters)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::WordEndPosition, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(onlyWordCharacters)));
	}

	/*
	 * Is the range start..end considered a word?
	 */
	bool Editor::IsRangeWord(int64_t start, int64_t end)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::IsRangeWord, static_cast<Scintilla::uptr_t>(start), static_cast<Scintilla::sptr_t>(end)));
	}

	/*
	 * Measure the pixel width of some text in a particular style.
	 * NUL terminated text argument.
	 * Does not handle tab or control characters.
	 */
	int32_t Editor::TextWidthFromBuffer(int32_t style, Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::TextWidth, static_cast<Scintilla::uptr_t>(style), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	int32_t Editor::TextWidth(int32_t style, hstring const &text)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::TextWidth, static_cast<Scintilla::uptr_t>(style), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str())));
	}

	/*
	 * Retrieve the height of a particular line of text in pixels.
	 */
	int32_t Editor::TextHeight(int64_t line)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::TextHeight, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Append a string to the end of the document without changing the selection.
	 */
	void Editor::AppendTextFromBuffer(int64_t length, Windows::Storage::Streams::IBuffer const &text)
	{
		_scintilla->WndProc(Scintilla::Message::AppendText, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr));
	}

	void Editor::AppendText(int64_t length, hstring const &text)
	{
		_scintilla->WndProc(Scintilla::Message::AppendText, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str()));
	}

	/*
	 * Join the lines in the target.
	 */
	void Editor::LinesJoin()
	{
		_scintilla->WndProc(Scintilla::Message::LinesJoin, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Split the lines in the target into lines that are less wide than pixelWidth
	 * where possible.
	 */
	void Editor::LinesSplit(int32_t pixelWidth)
	{
		_scintilla->WndProc(Scintilla::Message::LinesSplit, static_cast<Scintilla::uptr_t>(pixelWidth), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set one of the colours used as a chequerboard pattern in the fold margin
	 */
	void Editor::SetFoldMarginColour(bool useSetting, int32_t back)
	{
		_scintilla->WndProc(Scintilla::Message::SetFoldMarginColour, static_cast<Scintilla::uptr_t>(useSetting), static_cast<Scintilla::sptr_t>(back));
	}

	/*
	 * Set the other colour used as a chequerboard pattern in the fold margin
	 */
	void Editor::SetFoldMarginHiColour(bool useSetting, int32_t fore)
	{
		_scintilla->WndProc(Scintilla::Message::SetFoldMarginHiColour, static_cast<Scintilla::uptr_t>(useSetting), static_cast<Scintilla::sptr_t>(fore));
	}

	/*
	 * Move caret down one line.
	 */
	void Editor::LineDown()
	{
		_scintilla->WndProc(Scintilla::Message::LineDown, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret down one line extending selection to new caret position.
	 */
	void Editor::LineDownExtend()
	{
		_scintilla->WndProc(Scintilla::Message::LineDownExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret up one line.
	 */
	void Editor::LineUp()
	{
		_scintilla->WndProc(Scintilla::Message::LineUp, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret up one line extending selection to new caret position.
	 */
	void Editor::LineUpExtend()
	{
		_scintilla->WndProc(Scintilla::Message::LineUpExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret left one character.
	 */
	void Editor::CharLeft()
	{
		_scintilla->WndProc(Scintilla::Message::CharLeft, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret left one character extending selection to new caret position.
	 */
	void Editor::CharLeftExtend()
	{
		_scintilla->WndProc(Scintilla::Message::CharLeftExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret right one character.
	 */
	void Editor::CharRight()
	{
		_scintilla->WndProc(Scintilla::Message::CharRight, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret right one character extending selection to new caret position.
	 */
	void Editor::CharRightExtend()
	{
		_scintilla->WndProc(Scintilla::Message::CharRightExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret left one word.
	 */
	void Editor::WordLeft()
	{
		_scintilla->WndProc(Scintilla::Message::WordLeft, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret left one word extending selection to new caret position.
	 */
	void Editor::WordLeftExtend()
	{
		_scintilla->WndProc(Scintilla::Message::WordLeftExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret right one word.
	 */
	void Editor::WordRight()
	{
		_scintilla->WndProc(Scintilla::Message::WordRight, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret right one word extending selection to new caret position.
	 */
	void Editor::WordRightExtend()
	{
		_scintilla->WndProc(Scintilla::Message::WordRightExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to first position on line.
	 */
	void Editor::Home()
	{
		_scintilla->WndProc(Scintilla::Message::Home, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to first position on line extending selection to new caret position.
	 */
	void Editor::HomeExtend()
	{
		_scintilla->WndProc(Scintilla::Message::HomeExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to last position on line.
	 */
	void Editor::LineEnd()
	{
		_scintilla->WndProc(Scintilla::Message::LineEnd, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to last position on line extending selection to new caret position.
	 */
	void Editor::LineEndExtend()
	{
		_scintilla->WndProc(Scintilla::Message::LineEndExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to first position in document.
	 */
	void Editor::DocumentStart()
	{
		_scintilla->WndProc(Scintilla::Message::DocumentStart, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to first position in document extending selection to new caret position.
	 */
	void Editor::DocumentStartExtend()
	{
		_scintilla->WndProc(Scintilla::Message::DocumentStartExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to last position in document.
	 */
	void Editor::DocumentEnd()
	{
		_scintilla->WndProc(Scintilla::Message::DocumentEnd, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to last position in document extending selection to new caret position.
	 */
	void Editor::DocumentEndExtend()
	{
		_scintilla->WndProc(Scintilla::Message::DocumentEndExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret one page up.
	 */
	void Editor::PageUp()
	{
		_scintilla->WndProc(Scintilla::Message::PageUp, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret one page up extending selection to new caret position.
	 */
	void Editor::PageUpExtend()
	{
		_scintilla->WndProc(Scintilla::Message::PageUpExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret one page down.
	 */
	void Editor::PageDown()
	{
		_scintilla->WndProc(Scintilla::Message::PageDown, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret one page down extending selection to new caret position.
	 */
	void Editor::PageDownExtend()
	{
		_scintilla->WndProc(Scintilla::Message::PageDownExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Switch from insert to overtype mode or the reverse.
	 */
	void Editor::EditToggleOvertype()
	{
		_scintilla->WndProc(Scintilla::Message::EditToggleOvertype, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Cancel any modes such as call tip or auto-completion list display.
	 */
	void Editor::Cancel()
	{
		_scintilla->WndProc(Scintilla::Message::Cancel, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Delete the selection or if no selection, the character before the caret.
	 */
	void Editor::DeleteBack()
	{
		_scintilla->WndProc(Scintilla::Message::DeleteBack, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * If selection is empty or all on one line replace the selection with a tab character.
	 * If more than one line selected, indent the lines.
	 */
	void Editor::Tab()
	{
		_scintilla->WndProc(Scintilla::Message::Tab, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Dedent the selected lines.
	 */
	void Editor::BackTab()
	{
		_scintilla->WndProc(Scintilla::Message::BackTab, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Insert a new line, may use a CRLF, CR or LF depending on EOL mode.
	 */
	void Editor::NewLine()
	{
		_scintilla->WndProc(Scintilla::Message::NewLine, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Insert a Form Feed character.
	 */
	void Editor::FormFeed()
	{
		_scintilla->WndProc(Scintilla::Message::FormFeed, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to before first visible character on line.
	 * If already there move to first character on line.
	 */
	void Editor::VCHome()
	{
		_scintilla->WndProc(Scintilla::Message::VCHome, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Like VCHome but extending selection to new caret position.
	 */
	void Editor::VCHomeExtend()
	{
		_scintilla->WndProc(Scintilla::Message::VCHomeExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Magnify the displayed text by increasing the sizes by 1 point.
	 */
	void Editor::ZoomIn()
	{
		_scintilla->WndProc(Scintilla::Message::ZoomIn, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Make the displayed text smaller by decreasing the sizes by 1 point.
	 */
	void Editor::ZoomOut()
	{
		_scintilla->WndProc(Scintilla::Message::ZoomOut, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Delete the word to the left of the caret.
	 */
	void Editor::DelWordLeft()
	{
		_scintilla->WndProc(Scintilla::Message::DelWordLeft, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Delete the word to the right of the caret.
	 */
	void Editor::DelWordRight()
	{
		_scintilla->WndProc(Scintilla::Message::DelWordRight, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Delete the word to the right of the caret, but not the trailing non-word characters.
	 */
	void Editor::DelWordRightEnd()
	{
		_scintilla->WndProc(Scintilla::Message::DelWordRightEnd, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Cut the line containing the caret.
	 */
	void Editor::LineCut()
	{
		_scintilla->WndProc(Scintilla::Message::LineCut, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Delete the line containing the caret.
	 */
	void Editor::LineDelete()
	{
		_scintilla->WndProc(Scintilla::Message::LineDelete, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Switch the current line with the previous.
	 */
	void Editor::LineTranspose()
	{
		_scintilla->WndProc(Scintilla::Message::LineTranspose, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Reverse order of selected lines.
	 */
	void Editor::LineReverse()
	{
		_scintilla->WndProc(Scintilla::Message::LineReverse, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Duplicate the current line.
	 */
	void Editor::LineDuplicate()
	{
		_scintilla->WndProc(Scintilla::Message::LineDuplicate, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Transform the selection to lower case.
	 */
	void Editor::LowerCase()
	{
		_scintilla->WndProc(Scintilla::Message::LowerCase, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Transform the selection to upper case.
	 */
	void Editor::UpperCase()
	{
		_scintilla->WndProc(Scintilla::Message::UpperCase, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Scroll the document down, keeping the caret visible.
	 */
	void Editor::LineScrollDown()
	{
		_scintilla->WndProc(Scintilla::Message::LineScrollDown, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Scroll the document up, keeping the caret visible.
	 */
	void Editor::LineScrollUp()
	{
		_scintilla->WndProc(Scintilla::Message::LineScrollUp, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Delete the selection or if no selection, the character before the caret.
	 * Will not delete the character before at the start of a line.
	 */
	void Editor::DeleteBackNotLine()
	{
		_scintilla->WndProc(Scintilla::Message::DeleteBackNotLine, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to first position on display line.
	 */
	void Editor::HomeDisplay()
	{
		_scintilla->WndProc(Scintilla::Message::HomeDisplay, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to first position on display line extending selection to
	 * new caret position.
	 */
	void Editor::HomeDisplayExtend()
	{
		_scintilla->WndProc(Scintilla::Message::HomeDisplayExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to last position on display line.
	 */
	void Editor::LineEndDisplay()
	{
		_scintilla->WndProc(Scintilla::Message::LineEndDisplay, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to last position on display line extending selection to new
	 * caret position.
	 */
	void Editor::LineEndDisplayExtend()
	{
		_scintilla->WndProc(Scintilla::Message::LineEndDisplayExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Like Home but when word-wrap is enabled goes first to start of display line
	 * HomeDisplay, then to start of document line Home.
	 */
	void Editor::HomeWrap()
	{
		_scintilla->WndProc(Scintilla::Message::HomeWrap, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Like HomeExtend but when word-wrap is enabled extends first to start of display line
	 * HomeDisplayExtend, then to start of document line HomeExtend.
	 */
	void Editor::HomeWrapExtend()
	{
		_scintilla->WndProc(Scintilla::Message::HomeWrapExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Like LineEnd but when word-wrap is enabled goes first to end of display line
	 * LineEndDisplay, then to start of document line LineEnd.
	 */
	void Editor::LineEndWrap()
	{
		_scintilla->WndProc(Scintilla::Message::LineEndWrap, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Like LineEndExtend but when word-wrap is enabled extends first to end of display line
	 * LineEndDisplayExtend, then to start of document line LineEndExtend.
	 */
	void Editor::LineEndWrapExtend()
	{
		_scintilla->WndProc(Scintilla::Message::LineEndWrapExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Like VCHome but when word-wrap is enabled goes first to start of display line
	 * VCHomeDisplay, then behaves like VCHome.
	 */
	void Editor::VCHomeWrap()
	{
		_scintilla->WndProc(Scintilla::Message::VCHomeWrap, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Like VCHomeExtend but when word-wrap is enabled extends first to start of display line
	 * VCHomeDisplayExtend, then behaves like VCHomeExtend.
	 */
	void Editor::VCHomeWrapExtend()
	{
		_scintilla->WndProc(Scintilla::Message::VCHomeWrapExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Copy the line containing the caret.
	 */
	void Editor::LineCopy()
	{
		_scintilla->WndProc(Scintilla::Message::LineCopy, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move the caret inside current view if it's not there already.
	 */
	void Editor::MoveCaretInsideView()
	{
		_scintilla->WndProc(Scintilla::Message::MoveCaretInsideView, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * How many characters are on a line, including end of line characters?
	 */
	int64_t Editor::LineLength(int64_t line)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::LineLength, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Highlight the characters at two positions.
	 */
	void Editor::BraceHighlight(int64_t posA, int64_t posB)
	{
		_scintilla->WndProc(Scintilla::Message::BraceHighlight, static_cast<Scintilla::uptr_t>(posA), static_cast<Scintilla::sptr_t>(posB));
	}

	/*
	 * Use specified indicator to highlight matching braces instead of changing their style.
	 */
	void Editor::BraceHighlightIndicator(bool useSetting, int32_t indicator)
	{
		_scintilla->WndProc(Scintilla::Message::BraceHighlightIndicator, static_cast<Scintilla::uptr_t>(useSetting), static_cast<Scintilla::sptr_t>(indicator));
	}

	/*
	 * Highlight the character at a position indicating there is no matching brace.
	 */
	void Editor::BraceBadLight(int64_t pos)
	{
		_scintilla->WndProc(Scintilla::Message::BraceBadLight, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Use specified indicator to highlight non matching brace instead of changing its style.
	 */
	void Editor::BraceBadLightIndicator(bool useSetting, int32_t indicator)
	{
		_scintilla->WndProc(Scintilla::Message::BraceBadLightIndicator, static_cast<Scintilla::uptr_t>(useSetting), static_cast<Scintilla::sptr_t>(indicator));
	}

	/*
	 * Find the position of a matching brace or INVALID_POSITION if no match.
	 * The maxReStyle must be 0 for now. It may be defined in a future release.
	 */
	int64_t Editor::BraceMatch(int64_t pos, int32_t maxReStyle)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::BraceMatch, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(maxReStyle)));
	}

	/*
	 * Similar to BraceMatch, but matching starts at the explicit start position.
	 */
	int64_t Editor::BraceMatchNext(int64_t pos, int64_t startPos)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::BraceMatchNext, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(startPos)));
	}

	/*
	 * Add a new vertical edge to the view.
	 */
	void Editor::MultiEdgeAddLine(int64_t column, int32_t edgeColour)
	{
		_scintilla->WndProc(Scintilla::Message::MultiEdgeAddLine, static_cast<Scintilla::uptr_t>(column), static_cast<Scintilla::sptr_t>(edgeColour));
	}

	/*
	 * Clear all vertical edges.
	 */
	void Editor::MultiEdgeClearAll()
	{
		_scintilla->WndProc(Scintilla::Message::MultiEdgeClearAll, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Sets the current caret position to be the search anchor.
	 */
	void Editor::SearchAnchor()
	{
		_scintilla->WndProc(Scintilla::Message::SearchAnchor, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Find some text starting at the search anchor.
	 * Does not ensure the selection is visible.
	 */
	int64_t Editor::SearchNextFromBuffer(MicaEditor::FindOption const &searchFlags, Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::SearchNext, static_cast<Scintilla::uptr_t>(searchFlags), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	int64_t Editor::SearchNext(MicaEditor::FindOption const &searchFlags, hstring const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::SearchNext, static_cast<Scintilla::uptr_t>(searchFlags), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str())));
	}

	/*
	 * Find some text starting at the search anchor and moving backwards.
	 * Does not ensure the selection is visible.
	 */
	int64_t Editor::SearchPrevFromBuffer(MicaEditor::FindOption const &searchFlags, Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::SearchPrev, static_cast<Scintilla::uptr_t>(searchFlags), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	int64_t Editor::SearchPrev(MicaEditor::FindOption const &searchFlags, hstring const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::SearchPrev, static_cast<Scintilla::uptr_t>(searchFlags), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str())));
	}

	/*
	 * Set whether a pop up menu is displayed automatically when the user presses
	 * the wrong mouse button on certain areas.
	 */
	void Editor::UsePopUp(MicaEditor::PopUp const &popUpMode)
	{
		_scintilla->WndProc(Scintilla::Message::UsePopUp, static_cast<Scintilla::uptr_t>(popUpMode), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Create a new document object.
	 * Starts with reference count of 1 and not selected into editor.
	 */
	uint64_t Editor::CreateDocument(int64_t bytes, MicaEditor::DocumentOption const &documentOptions)
	{
		return static_cast<uint64_t>(_scintilla->WndProc(Scintilla::Message::CreateDocument, static_cast<Scintilla::uptr_t>(bytes), static_cast<Scintilla::sptr_t>(documentOptions)));
	}

	/*
	 * Extend life of document.
	 */
	void Editor::AddRefDocument(uint64_t doc)
	{
		_scintilla->WndProc(Scintilla::Message::AddRefDocument, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(doc));
	}

	/*
	 * Release a reference to the document, deleting document if it fades to black.
	 */
	void Editor::ReleaseDocument(uint64_t doc)
	{
		_scintilla->WndProc(Scintilla::Message::ReleaseDocument, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(doc));
	}

	/*
	 * Move to the previous change in capitalisation.
	 */
	void Editor::WordPartLeft()
	{
		_scintilla->WndProc(Scintilla::Message::WordPartLeft, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move to the previous change in capitalisation extending selection
	 * to new caret position.
	 */
	void Editor::WordPartLeftExtend()
	{
		_scintilla->WndProc(Scintilla::Message::WordPartLeftExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move to the change next in capitalisation.
	 */
	void Editor::WordPartRight()
	{
		_scintilla->WndProc(Scintilla::Message::WordPartRight, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move to the next change in capitalisation extending selection
	 * to new caret position.
	 */
	void Editor::WordPartRightExtend()
	{
		_scintilla->WndProc(Scintilla::Message::WordPartRightExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the way the display area is determined when a particular line
	 * is to be moved to by Find, FindNext, GotoLine, etc.
	 */
	void Editor::SetVisiblePolicy(MicaEditor::VisiblePolicy const &visiblePolicy, int32_t visibleSlop)
	{
		_scintilla->WndProc(Scintilla::Message::SetVisiblePolicy, static_cast<Scintilla::uptr_t>(visiblePolicy), static_cast<Scintilla::sptr_t>(visibleSlop));
	}

	/*
	 * Delete back from the current position to the start of the line.
	 */
	void Editor::DelLineLeft()
	{
		_scintilla->WndProc(Scintilla::Message::DelLineLeft, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Delete forwards from the current position to the end of the line.
	 */
	void Editor::DelLineRight()
	{
		_scintilla->WndProc(Scintilla::Message::DelLineRight, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the last x chosen value to be the caret x position.
	 */
	void Editor::ChooseCaretX()
	{
		_scintilla->WndProc(Scintilla::Message::ChooseCaretX, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the focus to this Scintilla widget.
	 */
	void Editor::GrabFocus()
	{
		_scintilla->WndProc(Scintilla::Message::GrabFocus, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the way the caret is kept visible when going sideways.
	 * The exclusion zone is given in pixels.
	 */
	void Editor::SetXCaretPolicy(MicaEditor::CaretPolicy const &caretPolicy, int32_t caretSlop)
	{
		_scintilla->WndProc(Scintilla::Message::SetXCaretPolicy, static_cast<Scintilla::uptr_t>(caretPolicy), static_cast<Scintilla::sptr_t>(caretSlop));
	}

	/*
	 * Set the way the line the caret is on is kept visible.
	 * The exclusion zone is given in lines.
	 */
	void Editor::SetYCaretPolicy(MicaEditor::CaretPolicy const &caretPolicy, int32_t caretSlop)
	{
		_scintilla->WndProc(Scintilla::Message::SetYCaretPolicy, static_cast<Scintilla::uptr_t>(caretPolicy), static_cast<Scintilla::sptr_t>(caretSlop));
	}

	/*
	 * Move caret down one paragraph (delimited by empty lines).
	 */
	void Editor::ParaDown()
	{
		_scintilla->WndProc(Scintilla::Message::ParaDown, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Extend selection down one paragraph (delimited by empty lines).
	 */
	void Editor::ParaDownExtend()
	{
		_scintilla->WndProc(Scintilla::Message::ParaDownExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret up one paragraph (delimited by empty lines).
	 */
	void Editor::ParaUp()
	{
		_scintilla->WndProc(Scintilla::Message::ParaUp, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Extend selection up one paragraph (delimited by empty lines).
	 */
	void Editor::ParaUpExtend()
	{
		_scintilla->WndProc(Scintilla::Message::ParaUpExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Given a valid document position, return the previous position taking code
	 * page into account. Returns 0 if passed 0.
	 */
	int64_t Editor::PositionBefore(int64_t pos)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::PositionBefore, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Given a valid document position, return the next position taking code
	 * page into account. Maximum value returned is the last position in the document.
	 */
	int64_t Editor::PositionAfter(int64_t pos)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::PositionAfter, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Given a valid document position, return a position that differs in a number
	 * of characters. Returned value is always between 0 and last position in document.
	 */
	int64_t Editor::PositionRelative(int64_t pos, int64_t relative)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::PositionRelative, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(relative)));
	}

	/*
	 * Given a valid document position, return a position that differs in a number
	 * of UTF-16 code units. Returned value is always between 0 and last position in document.
	 * The result may point half way (2 bytes) inside a non-BMP character.
	 */
	int64_t Editor::PositionRelativeCodeUnits(int64_t pos, int64_t relative)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::PositionRelativeCodeUnits, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(relative)));
	}

	/*
	 * Copy a range of text to the clipboard. Positions are clipped into the document.
	 */
	void Editor::CopyRange(int64_t start, int64_t end)
	{
		_scintilla->WndProc(Scintilla::Message::CopyRange, static_cast<Scintilla::uptr_t>(start), static_cast<Scintilla::sptr_t>(end));
	}

	/*
	 * Copy argument text to the clipboard.
	 */
	void Editor::CopyTextFromBuffer(int64_t length, Windows::Storage::Streams::IBuffer const &text)
	{
		_scintilla->WndProc(Scintilla::Message::CopyText, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr));
	}

	void Editor::CopyText(int64_t length, hstring const &text)
	{
		_scintilla->WndProc(Scintilla::Message::CopyText, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str()));
	}

	/*
	 * Retrieve the position of the start of the selection at the given line (INVALID_POSITION if no selection on this line).
	 */
	int64_t Editor::GetLineSelStartPosition(int64_t line)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetLineSelStartPosition, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the position of the end of the selection at the given line (INVALID_POSITION if no selection on this line).
	 */
	int64_t Editor::GetLineSelEndPosition(int64_t line)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetLineSelEndPosition, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Move caret down one line, extending rectangular selection to new caret position.
	 */
	void Editor::LineDownRectExtend()
	{
		_scintilla->WndProc(Scintilla::Message::LineDownRectExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret up one line, extending rectangular selection to new caret position.
	 */
	void Editor::LineUpRectExtend()
	{
		_scintilla->WndProc(Scintilla::Message::LineUpRectExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret left one character, extending rectangular selection to new caret position.
	 */
	void Editor::CharLeftRectExtend()
	{
		_scintilla->WndProc(Scintilla::Message::CharLeftRectExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret right one character, extending rectangular selection to new caret position.
	 */
	void Editor::CharRightRectExtend()
	{
		_scintilla->WndProc(Scintilla::Message::CharRightRectExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to first position on line, extending rectangular selection to new caret position.
	 */
	void Editor::HomeRectExtend()
	{
		_scintilla->WndProc(Scintilla::Message::HomeRectExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to before first visible character on line.
	 * If already there move to first character on line.
	 * In either case, extend rectangular selection to new caret position.
	 */
	void Editor::VCHomeRectExtend()
	{
		_scintilla->WndProc(Scintilla::Message::VCHomeRectExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to last position on line, extending rectangular selection to new caret position.
	 */
	void Editor::LineEndRectExtend()
	{
		_scintilla->WndProc(Scintilla::Message::LineEndRectExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret one page up, extending rectangular selection to new caret position.
	 */
	void Editor::PageUpRectExtend()
	{
		_scintilla->WndProc(Scintilla::Message::PageUpRectExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret one page down, extending rectangular selection to new caret position.
	 */
	void Editor::PageDownRectExtend()
	{
		_scintilla->WndProc(Scintilla::Message::PageDownRectExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to top of page, or one page up if already at top of page.
	 */
	void Editor::StutteredPageUp()
	{
		_scintilla->WndProc(Scintilla::Message::StutteredPageUp, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to top of page, or one page up if already at top of page, extending selection to new caret position.
	 */
	void Editor::StutteredPageUpExtend()
	{
		_scintilla->WndProc(Scintilla::Message::StutteredPageUpExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to bottom of page, or one page down if already at bottom of page.
	 */
	void Editor::StutteredPageDown()
	{
		_scintilla->WndProc(Scintilla::Message::StutteredPageDown, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to bottom of page, or one page down if already at bottom of page, extending selection to new caret position.
	 */
	void Editor::StutteredPageDownExtend()
	{
		_scintilla->WndProc(Scintilla::Message::StutteredPageDownExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret left one word, position cursor at end of word.
	 */
	void Editor::WordLeftEnd()
	{
		_scintilla->WndProc(Scintilla::Message::WordLeftEnd, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret left one word, position cursor at end of word, extending selection to new caret position.
	 */
	void Editor::WordLeftEndExtend()
	{
		_scintilla->WndProc(Scintilla::Message::WordLeftEndExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret right one word, position cursor at end of word.
	 */
	void Editor::WordRightEnd()
	{
		_scintilla->WndProc(Scintilla::Message::WordRightEnd, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret right one word, position cursor at end of word, extending selection to new caret position.
	 */
	void Editor::WordRightEndExtend()
	{
		_scintilla->WndProc(Scintilla::Message::WordRightEndExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Reset the set of characters for whitespace and word characters to the defaults.
	 */
	void Editor::SetCharsDefault()
	{
		_scintilla->WndProc(Scintilla::Message::SetCharsDefault, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Enlarge the document to a particular size of text bytes.
	 */
	void Editor::Allocate(int64_t bytes)
	{
		_scintilla->WndProc(Scintilla::Message::Allocate, static_cast<Scintilla::uptr_t>(bytes), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Returns the target converted to UTF8.
	 * Return the length in bytes.
	 */
	int64_t Editor::TargetAsUTF8WriteBuffer(Windows::Storage::Streams::IBuffer const &s)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::TargetAsUTF8, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(s ? s.data() : nullptr)));
	}

	hstring Editor::TargetAsUTF8()
	{
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::TargetAsUTF8, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::TargetAsUTF8, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Set the length of the utf8 argument for calling EncodedFromUTF8.
	 * Set to -1 and the string will be measured to the first nul.
	 */
	void Editor::SetLengthForEncode(int64_t bytes)
	{
		_scintilla->WndProc(Scintilla::Message::SetLengthForEncode, static_cast<Scintilla::uptr_t>(bytes), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Translates a UTF8 string into the document encoding.
	 * Return the length of the result in bytes.
	 * On error return 0.
	 */
	int64_t Editor::EncodedFromUTF8WriteBuffer(Windows::Storage::Streams::IBuffer const &utf8, Windows::Storage::Streams::IBuffer const &encoded)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::EncodedFromUTF8, reinterpret_cast<Scintilla::uptr_t>(utf8 ? utf8.data() : nullptr), reinterpret_cast<Scintilla::sptr_t>(encoded ? encoded.data() : nullptr)));
	}

	hstring Editor::EncodedFromUTF8(hstring const &utf8)
	{
		const auto wParam{ reinterpret_cast<Scintilla::uptr_t>(to_string(utf8).c_str()) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::EncodedFromUTF8, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::EncodedFromUTF8, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Find the position of a column on a line taking into account tabs and
	 * multi-byte characters. If beyond end of line, return line end position.
	 */
	int64_t Editor::FindColumn(int64_t line, int64_t column)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::FindColumn, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(column)));
	}

	/*
	 * Switch between sticky and non-sticky: meant to be bound to a key.
	 */
	void Editor::ToggleCaretSticky()
	{
		_scintilla->WndProc(Scintilla::Message::ToggleCaretSticky, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Replace the selection with text like a rectangular paste.
	 */
	void Editor::ReplaceRectangularFromBuffer(int64_t length, Windows::Storage::Streams::IBuffer const &text)
	{
		_scintilla->WndProc(Scintilla::Message::ReplaceRectangular, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr));
	}

	void Editor::ReplaceRectangular(int64_t length, hstring const &text)
	{
		_scintilla->WndProc(Scintilla::Message::ReplaceRectangular, static_cast<Scintilla::uptr_t>(length), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str()));
	}

	/*
	 * Duplicate the selection. If selection empty duplicate the line containing the caret.
	 */
	void Editor::SelectionDuplicate()
	{
		_scintilla->WndProc(Scintilla::Message::SelectionDuplicate, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Turn a indicator on over a range.
	 */
	void Editor::IndicatorFillRange(int64_t start, int64_t lengthFill)
	{
		_scintilla->WndProc(Scintilla::Message::IndicatorFillRange, static_cast<Scintilla::uptr_t>(start), static_cast<Scintilla::sptr_t>(lengthFill));
	}

	/*
	 * Turn a indicator off over a range.
	 */
	void Editor::IndicatorClearRange(int64_t start, int64_t lengthClear)
	{
		_scintilla->WndProc(Scintilla::Message::IndicatorClearRange, static_cast<Scintilla::uptr_t>(start), static_cast<Scintilla::sptr_t>(lengthClear));
	}

	/*
	 * Are any indicators present at pos?
	 */
	int32_t Editor::IndicatorAllOnFor(int64_t pos)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::IndicatorAllOnFor, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * What value does a particular indicator have at a position?
	 */
	int32_t Editor::IndicatorValueAt(int32_t indicator, int64_t pos)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::IndicatorValueAt, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(pos)));
	}

	/*
	 * Where does a particular indicator start?
	 */
	int64_t Editor::IndicatorStart(int32_t indicator, int64_t pos)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::IndicatorStart, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(pos)));
	}

	/*
	 * Where does a particular indicator end?
	 */
	int64_t Editor::IndicatorEnd(int32_t indicator, int64_t pos)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::IndicatorEnd, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(pos)));
	}

	/*
	 * Copy the selection, if selection empty copy the line with the caret
	 */
	void Editor::CopyAllowLine()
	{
		_scintilla->WndProc(Scintilla::Message::CopyAllowLine, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Which symbol was defined for markerNumber with MarkerDefine
	 */
	int32_t Editor::MarkerSymbolDefined(int32_t markerNumber)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::MarkerSymbolDefined, static_cast<Scintilla::uptr_t>(markerNumber), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Clear the margin text on all lines
	 */
	void Editor::MarginTextClearAll()
	{
		_scintilla->WndProc(Scintilla::Message::MarginTextClearAll, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Clear the annotations from all lines
	 */
	void Editor::AnnotationClearAll()
	{
		_scintilla->WndProc(Scintilla::Message::AnnotationClearAll, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Release all extended (>255) style numbers
	 */
	void Editor::ReleaseAllExtendedStyles()
	{
		_scintilla->WndProc(Scintilla::Message::ReleaseAllExtendedStyles, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Allocate some extended (>255) style numbers and return the start of the range
	 */
	int32_t Editor::AllocateExtendedStyles(int32_t numberStyles)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::AllocateExtendedStyles, static_cast<Scintilla::uptr_t>(numberStyles), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Add a container action to the undo stack
	 */
	void Editor::AddUndoAction(int32_t token, MicaEditor::UndoFlags const &flags)
	{
		_scintilla->WndProc(Scintilla::Message::AddUndoAction, static_cast<Scintilla::uptr_t>(token), static_cast<Scintilla::sptr_t>(flags));
	}

	/*
	 * Find the position of a character from a point within the window.
	 */
	int64_t Editor::CharPositionFromPoint(int32_t x, int32_t y)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::CharPositionFromPoint, static_cast<Scintilla::uptr_t>(x), static_cast<Scintilla::sptr_t>(y)));
	}

	/*
	 * Find the position of a character from a point within the window.
	 * Return INVALID_POSITION if not close to text.
	 */
	int64_t Editor::CharPositionFromPointClose(int32_t x, int32_t y)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::CharPositionFromPointClose, static_cast<Scintilla::uptr_t>(x), static_cast<Scintilla::sptr_t>(y)));
	}

	/*
	 * Clear selections to a single empty stream selection
	 */
	void Editor::ClearSelections()
	{
		_scintilla->WndProc(Scintilla::Message::ClearSelections, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set a simple selection
	 */
	void Editor::SetSelection(int64_t caret, int64_t anchor)
	{
		_scintilla->WndProc(Scintilla::Message::SetSelection, static_cast<Scintilla::uptr_t>(caret), static_cast<Scintilla::sptr_t>(anchor));
	}

	/*
	 * Add a selection
	 */
	void Editor::AddSelection(int64_t caret, int64_t anchor)
	{
		_scintilla->WndProc(Scintilla::Message::AddSelection, static_cast<Scintilla::uptr_t>(caret), static_cast<Scintilla::sptr_t>(anchor));
	}

	/*
	 * Drop one selection
	 */
	void Editor::DropSelectionN(int32_t selection)
	{
		_scintilla->WndProc(Scintilla::Message::DropSelectionN, static_cast<Scintilla::uptr_t>(selection), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the main selection to the next selection.
	 */
	void Editor::RotateSelection()
	{
		_scintilla->WndProc(Scintilla::Message::RotateSelection, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Swap that caret and anchor of the main selection.
	 */
	void Editor::SwapMainAnchorCaret()
	{
		_scintilla->WndProc(Scintilla::Message::SwapMainAnchorCaret, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Add the next occurrence of the main selection to the set of selections as main.
	 * If the current selection is empty then select word around caret.
	 */
	void Editor::MultipleSelectAddNext()
	{
		_scintilla->WndProc(Scintilla::Message::MultipleSelectAddNext, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Add each occurrence of the main selection in the target to the set of selections.
	 * If the current selection is empty then select word around caret.
	 */
	void Editor::MultipleSelectAddEach()
	{
		_scintilla->WndProc(Scintilla::Message::MultipleSelectAddEach, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Indicate that the internal state of a lexer has changed over a range and therefore
	 * there may be a need to redraw.
	 */
	int32_t Editor::ChangeLexerState(int64_t start, int64_t end)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::ChangeLexerState, static_cast<Scintilla::uptr_t>(start), static_cast<Scintilla::sptr_t>(end)));
	}

	/*
	 * Find the next line at or after lineStart that is a contracted fold header line.
	 * Return -1 when no more lines.
	 */
	int64_t Editor::ContractedFoldNext(int64_t lineStart)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::ContractedFoldNext, static_cast<Scintilla::uptr_t>(lineStart), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Centre current line in window.
	 */
	void Editor::VerticalCentreCaret()
	{
		_scintilla->WndProc(Scintilla::Message::VerticalCentreCaret, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move the selected lines up one line, shifting the line above after the selection
	 */
	void Editor::MoveSelectedLinesUp()
	{
		_scintilla->WndProc(Scintilla::Message::MoveSelectedLinesUp, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move the selected lines down one line, shifting the line below before the selection
	 */
	void Editor::MoveSelectedLinesDown()
	{
		_scintilla->WndProc(Scintilla::Message::MoveSelectedLinesDown, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Define a marker from RGBA data.
	 * It has the width and height from RGBAImageSetWidth/Height
	 */
	void Editor::MarkerDefineRGBAImageFromBuffer(int32_t markerNumber, Windows::Storage::Streams::IBuffer const &pixels)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerDefineRGBAImage, static_cast<Scintilla::uptr_t>(markerNumber), reinterpret_cast<Scintilla::sptr_t>(pixels ? pixels.data() : nullptr));
	}

	void Editor::MarkerDefineRGBAImage(int32_t markerNumber, hstring const &pixels)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerDefineRGBAImage, static_cast<Scintilla::uptr_t>(markerNumber), reinterpret_cast<Scintilla::sptr_t>(to_string(pixels).c_str()));
	}

	/*
	 * Register an RGBA image for use in autocompletion lists.
	 * It has the width and height from RGBAImageSetWidth/Height
	 */
	void Editor::RegisterRGBAImageFromBuffer(int32_t type, Windows::Storage::Streams::IBuffer const &pixels)
	{
		_scintilla->WndProc(Scintilla::Message::RegisterRGBAImage, static_cast<Scintilla::uptr_t>(type), reinterpret_cast<Scintilla::sptr_t>(pixels ? pixels.data() : nullptr));
	}

	void Editor::RegisterRGBAImage(int32_t type, hstring const &pixels)
	{
		_scintilla->WndProc(Scintilla::Message::RegisterRGBAImage, static_cast<Scintilla::uptr_t>(type), reinterpret_cast<Scintilla::sptr_t>(to_string(pixels).c_str()));
	}

	/*
	 * Scroll to start of document.
	 */
	void Editor::ScrollToStart()
	{
		_scintilla->WndProc(Scintilla::Message::ScrollToStart, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Scroll to end of document.
	 */
	void Editor::ScrollToEnd()
	{
		_scintilla->WndProc(Scintilla::Message::ScrollToEnd, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Create an ILoader*.
	 */
	uint64_t Editor::CreateLoader(int64_t bytes, MicaEditor::DocumentOption const &documentOptions)
	{
		return static_cast<uint64_t>(_scintilla->WndProc(Scintilla::Message::CreateLoader, static_cast<Scintilla::uptr_t>(bytes), static_cast<Scintilla::sptr_t>(documentOptions)));
	}

	/*
	 * On macOS, show a find indicator.
	 */
	void Editor::FindIndicatorShow(int64_t start, int64_t end)
	{
		_scintilla->WndProc(Scintilla::Message::FindIndicatorShow, static_cast<Scintilla::uptr_t>(start), static_cast<Scintilla::sptr_t>(end));
	}

	/*
	 * On macOS, flash a find indicator, then fade out.
	 */
	void Editor::FindIndicatorFlash(int64_t start, int64_t end)
	{
		_scintilla->WndProc(Scintilla::Message::FindIndicatorFlash, static_cast<Scintilla::uptr_t>(start), static_cast<Scintilla::sptr_t>(end));
	}

	/*
	 * On macOS, hide the find indicator.
	 */
	void Editor::FindIndicatorHide()
	{
		_scintilla->WndProc(Scintilla::Message::FindIndicatorHide, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Move caret to before first visible character on display line.
	 * If already there move to first character on display line.
	 */
	void Editor::VCHomeDisplay()
	{
		_scintilla->WndProc(Scintilla::Message::VCHomeDisplay, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Like VCHomeDisplay but extending selection to new caret position.
	 */
	void Editor::VCHomeDisplayExtend()
	{
		_scintilla->WndProc(Scintilla::Message::VCHomeDisplayExtend, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Remove a character representation.
	 */
	void Editor::ClearRepresentationFromBuffer(Windows::Storage::Streams::IBuffer const &encodedCharacter)
	{
		_scintilla->WndProc(Scintilla::Message::ClearRepresentation, reinterpret_cast<Scintilla::uptr_t>(encodedCharacter ? encodedCharacter.data() : nullptr), static_cast<Scintilla::sptr_t>(0));
	}

	void Editor::ClearRepresentation(hstring const &encodedCharacter)
	{
		_scintilla->WndProc(Scintilla::Message::ClearRepresentation, reinterpret_cast<Scintilla::uptr_t>(to_string(encodedCharacter).c_str()), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Clear representations to default.
	 */
	void Editor::ClearAllRepresentations()
	{
		_scintilla->WndProc(Scintilla::Message::ClearAllRepresentations, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Clear the end of annotations from all lines
	 */
	void Editor::EOLAnnotationClearAll()
	{
		_scintilla->WndProc(Scintilla::Message::EOLAnnotationClearAll, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Request line character index be created or its use count increased.
	 */
	void Editor::AllocateLineCharacterIndex(MicaEditor::LineCharacterIndexType const &lineCharacterIndex)
	{
		_scintilla->WndProc(Scintilla::Message::AllocateLineCharacterIndex, static_cast<Scintilla::uptr_t>(lineCharacterIndex), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Decrease use count of line character index and remove if 0.
	 */
	void Editor::ReleaseLineCharacterIndex(MicaEditor::LineCharacterIndexType const &lineCharacterIndex)
	{
		_scintilla->WndProc(Scintilla::Message::ReleaseLineCharacterIndex, static_cast<Scintilla::uptr_t>(lineCharacterIndex), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the document line containing a position measured in index units.
	 */
	int64_t Editor::LineFromIndexPosition(int64_t pos, MicaEditor::LineCharacterIndexType const &lineCharacterIndex)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::LineFromIndexPosition, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(lineCharacterIndex)));
	}

	/*
	 * Retrieve the position measured in index units at the start of a document line.
	 */
	int64_t Editor::IndexPositionFromLine(int64_t line, MicaEditor::LineCharacterIndexType const &lineCharacterIndex)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::IndexPositionFromLine, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(lineCharacterIndex)));
	}

	/*
	 * Start notifying the container of all key presses and commands.
	 */
	void Editor::StartRecord()
	{
		_scintilla->WndProc(Scintilla::Message::StartRecord, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Stop notifying the container of all key presses and commands.
	 */
	void Editor::StopRecord()
	{
		_scintilla->WndProc(Scintilla::Message::StopRecord, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Colourise a segment of the document using the current lexing language.
	 */
	void Editor::Colourise(int64_t start, int64_t end)
	{
		_scintilla->WndProc(Scintilla::Message::Colourise, static_cast<Scintilla::uptr_t>(start), static_cast<Scintilla::sptr_t>(end));
	}

	/*
	 * For private communication between an application and a known lexer.
	 */
	uint64_t Editor::PrivateLexerCall(int32_t operation, uint64_t pointer)
	{
		return static_cast<uint64_t>(_scintilla->WndProc(Scintilla::Message::PrivateLexerCall, static_cast<Scintilla::uptr_t>(operation), static_cast<Scintilla::sptr_t>(pointer)));
	}

	/*
	 * Retrieve a '\n' separated list of properties understood by the current lexer.
	 * Result is NUL-terminated.
	 */
	int32_t Editor::PropertyNamesWriteBuffer(Windows::Storage::Streams::IBuffer const &names)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::PropertyNames, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(names ? names.data() : nullptr)));
	}

	hstring Editor::PropertyNames()
	{
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::PropertyNames, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::PropertyNames, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Retrieve the type of a property.
	 */
	MicaEditor::TypeProperty Editor::PropertyTypeFromBuffer(Windows::Storage::Streams::IBuffer const &name)
	{
		return static_cast<MicaEditor::TypeProperty>(_scintilla->WndProc(Scintilla::Message::PropertyType, reinterpret_cast<Scintilla::uptr_t>(name ? name.data() : nullptr), static_cast<Scintilla::sptr_t>(0)));
	}

	MicaEditor::TypeProperty Editor::PropertyType(hstring const &name)
	{
		return static_cast<MicaEditor::TypeProperty>(_scintilla->WndProc(Scintilla::Message::PropertyType, reinterpret_cast<Scintilla::uptr_t>(to_string(name).c_str()), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Describe a property.
	 * Result is NUL-terminated.
	 */
	int32_t Editor::DescribePropertyWriteBuffer(Windows::Storage::Streams::IBuffer const &name, Windows::Storage::Streams::IBuffer const &description)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::DescribeProperty, reinterpret_cast<Scintilla::uptr_t>(name ? name.data() : nullptr), reinterpret_cast<Scintilla::sptr_t>(description ? description.data() : nullptr)));
	}

	hstring Editor::DescribeProperty(hstring const &name)
	{
		const auto wParam{ reinterpret_cast<Scintilla::uptr_t>(to_string(name).c_str()) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::DescribeProperty, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::DescribeProperty, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Retrieve a '\n' separated list of descriptions of the keyword sets understood by the current lexer.
	 * Result is NUL-terminated.
	 */
	int32_t Editor::DescribeKeyWordSetsWriteBuffer(Windows::Storage::Streams::IBuffer const &descriptions)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::DescribeKeyWordSets, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(descriptions ? descriptions.data() : nullptr)));
	}

	hstring Editor::DescribeKeyWordSets()
	{
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::DescribeKeyWordSets, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::DescribeKeyWordSets, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Allocate a set of sub styles for a particular base style, returning start of range
	 */
	int32_t Editor::AllocateSubStyles(int32_t styleBase, int32_t numberStyles)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::AllocateSubStyles, static_cast<Scintilla::uptr_t>(styleBase), static_cast<Scintilla::sptr_t>(numberStyles)));
	}

	/*
	 * Free allocated sub styles
	 */
	void Editor::FreeSubStyles()
	{
		_scintilla->WndProc(Scintilla::Message::FreeSubStyles, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Retrieve the name of a style.
	 * Result is NUL-terminated.
	 */
	int32_t Editor::NameOfStyleWriteBuffer(int32_t style, Windows::Storage::Streams::IBuffer const &name)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::NameOfStyle, static_cast<Scintilla::uptr_t>(style), reinterpret_cast<Scintilla::sptr_t>(name ? name.data() : nullptr)));
	}

	hstring Editor::NameOfStyle(int32_t style)
	{
		const auto wParam{ static_cast<Scintilla::uptr_t>(style) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::NameOfStyle, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::NameOfStyle, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Retrieve a ' ' separated list of style tags like "literal quoted string".
	 * Result is NUL-terminated.
	 */
	int32_t Editor::TagsOfStyleWriteBuffer(int32_t style, Windows::Storage::Streams::IBuffer const &tags)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::TagsOfStyle, static_cast<Scintilla::uptr_t>(style), reinterpret_cast<Scintilla::sptr_t>(tags ? tags.data() : nullptr)));
	}

	hstring Editor::TagsOfStyle(int32_t style)
	{
		const auto wParam{ static_cast<Scintilla::uptr_t>(style) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::TagsOfStyle, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::TagsOfStyle, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Retrieve a description of a style.
	 * Result is NUL-terminated.
	 */
	int32_t Editor::DescriptionOfStyleWriteBuffer(int32_t style, Windows::Storage::Streams::IBuffer const &description)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::DescriptionOfStyle, static_cast<Scintilla::uptr_t>(style), reinterpret_cast<Scintilla::sptr_t>(description ? description.data() : nullptr)));
	}

	hstring Editor::DescriptionOfStyle(int32_t style)
	{
		const auto wParam{ static_cast<Scintilla::uptr_t>(style) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::DescriptionOfStyle, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::DescriptionOfStyle, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Returns the character byte at the position.
	 */
	int32_t Editor::GetCharAt(int64_t pos)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetCharAt, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Returns the style byte at the position.
	 */
	int32_t Editor::GetStyleAt(int64_t pos)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetStyleAt, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Returns the unsigned style byte at the position.
	 */
	int32_t Editor::GetStyleIndexAt(int64_t pos)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetStyleIndexAt, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the locale for displaying text.
	 */
	int32_t Editor::GetFontLocaleWriteBuffer(Windows::Storage::Streams::IBuffer const &localeName)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetFontLocale, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(localeName ? localeName.data() : nullptr)));
	}

	hstring Editor::GetFontLocale()
	{
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetFontLocale, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetFontLocale, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Get the layer used for a marker that is drawn in the text area, not the margin.
	 */
	MicaEditor::Layer Editor::MarkerGetLayer(int32_t markerNumber)
	{
		return static_cast<MicaEditor::Layer>(_scintilla->WndProc(Scintilla::Message::MarkerGetLayer, static_cast<Scintilla::uptr_t>(markerNumber), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the type of a margin.
	 */
	MicaEditor::MarginType Editor::GetMarginTypeN(int32_t margin)
	{
		return static_cast<MicaEditor::MarginType>(_scintilla->WndProc(Scintilla::Message::GetMarginTypeN, static_cast<Scintilla::uptr_t>(margin), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the width of a margin in pixels.
	 */
	int32_t Editor::GetMarginWidthN(int32_t margin)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetMarginWidthN, static_cast<Scintilla::uptr_t>(margin), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the marker mask of a margin.
	 */
	int32_t Editor::GetMarginMaskN(int32_t margin)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetMarginMaskN, static_cast<Scintilla::uptr_t>(margin), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the mouse click sensitivity of a margin.
	 */
	bool Editor::GetMarginSensitiveN(int32_t margin)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetMarginSensitiveN, static_cast<Scintilla::uptr_t>(margin), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the cursor shown in a margin.
	 */
	MicaEditor::CursorShape Editor::GetMarginCursorN(int32_t margin)
	{
		return static_cast<MicaEditor::CursorShape>(_scintilla->WndProc(Scintilla::Message::GetMarginCursorN, static_cast<Scintilla::uptr_t>(margin), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the background colour of a margin
	 */
	int32_t Editor::GetMarginBackN(int32_t margin)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetMarginBackN, static_cast<Scintilla::uptr_t>(margin), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the foreground colour of a style.
	 */
	int32_t Editor::StyleGetFore(int32_t style)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::StyleGetFore, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the background colour of a style.
	 */
	int32_t Editor::StyleGetBack(int32_t style)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::StyleGetBack, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get is a style bold or not.
	 */
	bool Editor::StyleGetBold(int32_t style)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::StyleGetBold, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get is a style italic or not.
	 */
	bool Editor::StyleGetItalic(int32_t style)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::StyleGetItalic, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the size of characters of a style.
	 */
	int32_t Editor::StyleGetSize(int32_t style)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::StyleGetSize, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the font of a style.
	 * Returns the length of the fontName
	 * Result is NUL-terminated.
	 */
	int32_t Editor::StyleGetFontWriteBuffer(int32_t style, Windows::Storage::Streams::IBuffer const &fontName)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::StyleGetFont, static_cast<Scintilla::uptr_t>(style), reinterpret_cast<Scintilla::sptr_t>(fontName ? fontName.data() : nullptr)));
	}

	hstring Editor::StyleGetFont(int32_t style)
	{
		const auto wParam{ static_cast<Scintilla::uptr_t>(style) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::StyleGetFont, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::StyleGetFont, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Get is a style to have its end of line filled or not.
	 */
	bool Editor::StyleGetEOLFilled(int32_t style)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::StyleGetEOLFilled, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get is a style underlined or not.
	 */
	bool Editor::StyleGetUnderline(int32_t style)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::StyleGetUnderline, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get is a style mixed case, or to force upper or lower case.
	 */
	MicaEditor::CaseVisible Editor::StyleGetCase(int32_t style)
	{
		return static_cast<MicaEditor::CaseVisible>(_scintilla->WndProc(Scintilla::Message::StyleGetCase, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the character get of the font in a style.
	 */
	MicaEditor::CharacterSet Editor::StyleGetCharacterSet(int32_t style)
	{
		return static_cast<MicaEditor::CharacterSet>(_scintilla->WndProc(Scintilla::Message::StyleGetCharacterSet, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get is a style visible or not.
	 */
	bool Editor::StyleGetVisible(int32_t style)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::StyleGetVisible, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get is a style changeable or not (read only).
	 * Experimental feature, currently buggy.
	 */
	bool Editor::StyleGetChangeable(int32_t style)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::StyleGetChangeable, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get is a style a hotspot or not.
	 */
	bool Editor::StyleGetHotSpot(int32_t style)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::StyleGetHotSpot, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the size of characters of a style in points multiplied by 100
	 */
	int32_t Editor::StyleGetSizeFractional(int32_t style)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::StyleGetSizeFractional, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the weight of characters of a style.
	 */
	MicaEditor::FontWeight Editor::StyleGetWeight(int32_t style)
	{
		return static_cast<MicaEditor::FontWeight>(_scintilla->WndProc(Scintilla::Message::StyleGetWeight, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get whether a style may be monospaced.
	 */
	bool Editor::StyleGetCheckMonospaced(int32_t style)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::StyleGetCheckMonospaced, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the invisible representation for a style.
	 */
	int32_t Editor::StyleGetInvisibleRepresentationWriteBuffer(int32_t style, Windows::Storage::Streams::IBuffer const &representation)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::StyleGetInvisibleRepresentation, static_cast<Scintilla::uptr_t>(style), reinterpret_cast<Scintilla::sptr_t>(representation ? representation.data() : nullptr)));
	}

	hstring Editor::StyleGetInvisibleRepresentation(int32_t style)
	{
		const auto wParam{ static_cast<Scintilla::uptr_t>(style) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::StyleGetInvisibleRepresentation, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::StyleGetInvisibleRepresentation, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Get the colour of an element.
	 */
	int32_t Editor::GetElementColour(MicaEditor::Element const &element)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetElementColour, static_cast<Scintilla::uptr_t>(element), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get whether an element has been set by SetElementColour.
	 * When false, a platform-defined or default colour is used.
	 */
	bool Editor::GetElementIsSet(MicaEditor::Element const &element)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetElementIsSet, static_cast<Scintilla::uptr_t>(element), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get whether an element supports translucency.
	 */
	bool Editor::GetElementAllowsTranslucent(MicaEditor::Element const &element)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetElementAllowsTranslucent, static_cast<Scintilla::uptr_t>(element), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the colour of an element.
	 */
	int32_t Editor::GetElementBaseColour(MicaEditor::Element const &element)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetElementBaseColour, static_cast<Scintilla::uptr_t>(element), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the set of characters making up words for when moving or selecting by word.
	 * Returns the number of characters
	 */
	int32_t Editor::GetWordCharsWriteBuffer(Windows::Storage::Streams::IBuffer const &characters)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetWordChars, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(characters ? characters.data() : nullptr)));
	}

	hstring Editor::GetWordChars()
	{
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetWordChars, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetWordChars, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Retrieve the style of an indicator.
	 */
	MicaEditor::IndicatorStyle Editor::IndicGetStyle(int32_t indicator)
	{
		return static_cast<MicaEditor::IndicatorStyle>(_scintilla->WndProc(Scintilla::Message::IndicGetStyle, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the foreground colour of an indicator.
	 */
	int32_t Editor::IndicGetFore(int32_t indicator)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::IndicGetFore, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve whether indicator drawn under or over text.
	 */
	bool Editor::IndicGetUnder(int32_t indicator)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::IndicGetUnder, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the hover style of an indicator.
	 */
	MicaEditor::IndicatorStyle Editor::IndicGetHoverStyle(int32_t indicator)
	{
		return static_cast<MicaEditor::IndicatorStyle>(_scintilla->WndProc(Scintilla::Message::IndicGetHoverStyle, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the foreground hover colour of an indicator.
	 */
	int32_t Editor::IndicGetHoverFore(int32_t indicator)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::IndicGetHoverFore, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the attributes of an indicator.
	 */
	MicaEditor::IndicFlag Editor::IndicGetFlags(int32_t indicator)
	{
		return static_cast<MicaEditor::IndicFlag>(_scintilla->WndProc(Scintilla::Message::IndicGetFlags, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the stroke width of an indicator.
	 */
	int32_t Editor::IndicGetStrokeWidth(int32_t indicator)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::IndicGetStrokeWidth, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the extra styling information for a line.
	 */
	int32_t Editor::GetLineState(int64_t line)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetLineState, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the number of columns that a line is indented.
	 */
	int32_t Editor::GetLineIndentation(int64_t line)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetLineIndentation, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the position before the first non indentation character on a line.
	 */
	int64_t Editor::GetLineIndentPosition(int64_t line)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetLineIndentPosition, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the column number of a position, taking tab width into account.
	 */
	int64_t Editor::GetColumn(int64_t pos)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetColumn, static_cast<Scintilla::uptr_t>(pos), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the position after the last visible characters on a line.
	 */
	int64_t Editor::GetLineEndPosition(int64_t line)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetLineEndPosition, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the text in the target.
	 */
	int64_t Editor::GetTargetTextWriteBuffer(Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetTargetText, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	hstring Editor::GetTargetText()
	{
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetTargetText, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetTargetText, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Retrieve the fold level of a line.
	 */
	MicaEditor::FoldLevel Editor::GetFoldLevel(int64_t line)
	{
		return static_cast<MicaEditor::FoldLevel>(_scintilla->WndProc(Scintilla::Message::GetFoldLevel, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Find the last child line of a header line.
	 */
	int64_t Editor::GetLastChild(int64_t line, MicaEditor::FoldLevel const &level)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetLastChild, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(level)));
	}

	/*
	 * Find the parent line of a child line.
	 */
	int64_t Editor::GetFoldParent(int64_t line)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetFoldParent, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Is a line visible?
	 */
	bool Editor::GetLineVisible(int64_t line)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetLineVisible, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Is a header line expanded?
	 */
	bool Editor::GetFoldExpanded(int64_t line)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::GetFoldExpanded, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve the value of a tag from a regular expression search.
	 * Result is NUL-terminated.
	 */
	int32_t Editor::GetTagWriteBuffer(int32_t tagNumber, Windows::Storage::Streams::IBuffer const &tagValue)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetTag, static_cast<Scintilla::uptr_t>(tagNumber), reinterpret_cast<Scintilla::sptr_t>(tagValue ? tagValue.data() : nullptr)));
	}

	hstring Editor::GetTag(int32_t tagNumber)
	{
		const auto wParam{ static_cast<Scintilla::uptr_t>(tagNumber) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetTag, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetTag, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Get multi edge positions.
	 */
	int64_t Editor::GetMultiEdgeColumn(int32_t which)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetMultiEdgeColumn, static_cast<Scintilla::uptr_t>(which), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the fore colour for active hotspots.
	 */
	int32_t Editor::GetHotspotActiveFore()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetHotspotActiveFore, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the back colour for active hotspots.
	 */
	int32_t Editor::GetHotspotActiveBack()
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetHotspotActiveBack, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the set of characters making up whitespace for when moving or selecting by word.
	 */
	int32_t Editor::GetWhitespaceCharsWriteBuffer(Windows::Storage::Streams::IBuffer const &characters)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetWhitespaceChars, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(characters ? characters.data() : nullptr)));
	}

	hstring Editor::GetWhitespaceChars()
	{
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetWhitespaceChars, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetWhitespaceChars, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Get the set of characters making up punctuation characters
	 */
	int32_t Editor::GetPunctuationCharsWriteBuffer(Windows::Storage::Streams::IBuffer const &characters)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetPunctuationChars, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(characters ? characters.data() : nullptr)));
	}

	hstring Editor::GetPunctuationChars()
	{
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetPunctuationChars, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetPunctuationChars, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Get currently selected item text in the auto-completion list
	 * Returns the length of the item text
	 * Result is NUL-terminated.
	 */
	int32_t Editor::AutoCGetCurrentTextWriteBuffer(Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::AutoCGetCurrentText, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	hstring Editor::AutoCGetCurrentText()
	{
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::AutoCGetCurrentText, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::AutoCGetCurrentText, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Return a read-only pointer to a range of characters in the document.
	 * May move the gap so that the range is contiguous, but will only move up
	 * to lengthRange bytes.
	 */
	uint64_t Editor::GetRangePointer(int64_t start, int64_t lengthRange)
	{
		return static_cast<uint64_t>(_scintilla->WndProc(Scintilla::Message::GetRangePointer, static_cast<Scintilla::uptr_t>(start), static_cast<Scintilla::sptr_t>(lengthRange)));
	}

	/*
	 * Get the alpha fill colour of the given indicator.
	 */
	MicaEditor::Alpha Editor::IndicGetAlpha(int32_t indicator)
	{
		return static_cast<MicaEditor::Alpha>(_scintilla->WndProc(Scintilla::Message::IndicGetAlpha, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the alpha outline colour of the given indicator.
	 */
	MicaEditor::Alpha Editor::IndicGetOutlineAlpha(int32_t indicator)
	{
		return static_cast<MicaEditor::Alpha>(_scintilla->WndProc(Scintilla::Message::IndicGetOutlineAlpha, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the text in the text margin for a line
	 */
	int32_t Editor::MarginGetTextWriteBuffer(int64_t line, Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::MarginGetText, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	hstring Editor::MarginGetText(int64_t line)
	{
		const auto wParam{ static_cast<Scintilla::uptr_t>(line) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::MarginGetText, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::MarginGetText, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Get the style number for the text margin for a line
	 */
	int32_t Editor::MarginGetStyle(int64_t line)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::MarginGetStyle, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the styles in the text margin for a line
	 */
	int32_t Editor::MarginGetStylesWriteBuffer(int64_t line, Windows::Storage::Streams::IBuffer const &styles)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::MarginGetStyles, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(styles ? styles.data() : nullptr)));
	}

	hstring Editor::MarginGetStyles(int64_t line)
	{
		const auto wParam{ static_cast<Scintilla::uptr_t>(line) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::MarginGetStyles, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::MarginGetStyles, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Get the annotation text for a line
	 */
	int32_t Editor::AnnotationGetTextWriteBuffer(int64_t line, Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::AnnotationGetText, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	hstring Editor::AnnotationGetText(int64_t line)
	{
		const auto wParam{ static_cast<Scintilla::uptr_t>(line) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::AnnotationGetText, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::AnnotationGetText, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Get the style number for the annotations for a line
	 */
	int32_t Editor::AnnotationGetStyle(int64_t line)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::AnnotationGetStyle, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the annotation styles for a line
	 */
	int32_t Editor::AnnotationGetStylesWriteBuffer(int64_t line, Windows::Storage::Streams::IBuffer const &styles)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::AnnotationGetStyles, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(styles ? styles.data() : nullptr)));
	}

	hstring Editor::AnnotationGetStyles(int64_t line)
	{
		const auto wParam{ static_cast<Scintilla::uptr_t>(line) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::AnnotationGetStyles, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::AnnotationGetStyles, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Get the number of annotation lines for a line
	 */
	int32_t Editor::AnnotationGetLines(int64_t line)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::AnnotationGetLines, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Return the caret position of the nth selection.
	 */
	int64_t Editor::GetSelectionNCaret(int32_t selection)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetSelectionNCaret, static_cast<Scintilla::uptr_t>(selection), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Return the anchor position of the nth selection.
	 */
	int64_t Editor::GetSelectionNAnchor(int32_t selection)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetSelectionNAnchor, static_cast<Scintilla::uptr_t>(selection), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Return the virtual space of the caret of the nth selection.
	 */
	int64_t Editor::GetSelectionNCaretVirtualSpace(int32_t selection)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetSelectionNCaretVirtualSpace, static_cast<Scintilla::uptr_t>(selection), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Return the virtual space of the anchor of the nth selection.
	 */
	int64_t Editor::GetSelectionNAnchorVirtualSpace(int32_t selection)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetSelectionNAnchorVirtualSpace, static_cast<Scintilla::uptr_t>(selection), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Returns the position at the start of the selection.
	 */
	int64_t Editor::GetSelectionNStart(int32_t selection)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetSelectionNStart, static_cast<Scintilla::uptr_t>(selection), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Returns the virtual space at the start of the selection.
	 */
	int64_t Editor::GetSelectionNStartVirtualSpace(int32_t selection)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetSelectionNStartVirtualSpace, static_cast<Scintilla::uptr_t>(selection), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Returns the virtual space at the end of the selection.
	 */
	int64_t Editor::GetSelectionNEndVirtualSpace(int32_t selection)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetSelectionNEndVirtualSpace, static_cast<Scintilla::uptr_t>(selection), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Returns the position at the end of the selection.
	 */
	int64_t Editor::GetSelectionNEnd(int32_t selection)
	{
		return static_cast<int64_t>(_scintilla->WndProc(Scintilla::Message::GetSelectionNEnd, static_cast<Scintilla::uptr_t>(selection), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the way a character is drawn.
	 * Result is NUL-terminated.
	 */
	int32_t Editor::GetRepresentationWriteBuffer(Windows::Storage::Streams::IBuffer const &encodedCharacter, Windows::Storage::Streams::IBuffer const &representation)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetRepresentation, reinterpret_cast<Scintilla::uptr_t>(encodedCharacter ? encodedCharacter.data() : nullptr), reinterpret_cast<Scintilla::sptr_t>(representation ? representation.data() : nullptr)));
	}

	hstring Editor::GetRepresentation(hstring const &encodedCharacter)
	{
		const auto wParam{ reinterpret_cast<Scintilla::uptr_t>(to_string(encodedCharacter).c_str()) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetRepresentation, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetRepresentation, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Get the appearance of a representation.
	 */
	MicaEditor::RepresentationAppearance Editor::GetRepresentationAppearanceFromBuffer(Windows::Storage::Streams::IBuffer const &encodedCharacter)
	{
		return static_cast<MicaEditor::RepresentationAppearance>(_scintilla->WndProc(Scintilla::Message::GetRepresentationAppearance, reinterpret_cast<Scintilla::uptr_t>(encodedCharacter ? encodedCharacter.data() : nullptr), static_cast<Scintilla::sptr_t>(0)));
	}

	MicaEditor::RepresentationAppearance Editor::GetRepresentationAppearance(hstring const &encodedCharacter)
	{
		return static_cast<MicaEditor::RepresentationAppearance>(_scintilla->WndProc(Scintilla::Message::GetRepresentationAppearance, reinterpret_cast<Scintilla::uptr_t>(to_string(encodedCharacter).c_str()), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the colour of a representation.
	 */
	int32_t Editor::GetRepresentationColourFromBuffer(Windows::Storage::Streams::IBuffer const &encodedCharacter)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetRepresentationColour, reinterpret_cast<Scintilla::uptr_t>(encodedCharacter ? encodedCharacter.data() : nullptr), static_cast<Scintilla::sptr_t>(0)));
	}

	int32_t Editor::GetRepresentationColour(hstring const &encodedCharacter)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetRepresentationColour, reinterpret_cast<Scintilla::uptr_t>(to_string(encodedCharacter).c_str()), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the end of line annotation text for a line
	 */
	int32_t Editor::EOLAnnotationGetTextWriteBuffer(int64_t line, Windows::Storage::Streams::IBuffer const &text)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::EOLAnnotationGetText, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr)));
	}

	hstring Editor::EOLAnnotationGetText(int64_t line)
	{
		const auto wParam{ static_cast<Scintilla::uptr_t>(line) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::EOLAnnotationGetText, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::EOLAnnotationGetText, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Get the style number for the end of line annotations for a line
	 */
	int32_t Editor::EOLAnnotationGetStyle(int64_t line)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::EOLAnnotationGetStyle, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get whether a feature is supported
	 */
	bool Editor::SupportsFeature(MicaEditor::Supports const &feature)
	{
		return static_cast<bool>(_scintilla->WndProc(Scintilla::Message::SupportsFeature, static_cast<Scintilla::uptr_t>(feature), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Retrieve a "property" value previously set with SetProperty.
	 * Result is NUL-terminated.
	 */
	int32_t Editor::GetPropertyWriteBuffer(Windows::Storage::Streams::IBuffer const &key, Windows::Storage::Streams::IBuffer const &value)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetProperty, reinterpret_cast<Scintilla::uptr_t>(key ? key.data() : nullptr), reinterpret_cast<Scintilla::sptr_t>(value ? value.data() : nullptr)));
	}

	hstring Editor::GetProperty(hstring const &key)
	{
		const auto wParam{ reinterpret_cast<Scintilla::uptr_t>(to_string(key).c_str()) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetProperty, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetProperty, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Retrieve a "property" value previously set with SetProperty,
	 * with "$()" variable replacement on returned buffer.
	 * Result is NUL-terminated.
	 */
	int32_t Editor::GetPropertyExpandedWriteBuffer(Windows::Storage::Streams::IBuffer const &key, Windows::Storage::Streams::IBuffer const &value)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetPropertyExpanded, reinterpret_cast<Scintilla::uptr_t>(key ? key.data() : nullptr), reinterpret_cast<Scintilla::sptr_t>(value ? value.data() : nullptr)));
	}

	hstring Editor::GetPropertyExpanded(hstring const &key)
	{
		const auto wParam{ reinterpret_cast<Scintilla::uptr_t>(to_string(key).c_str()) };
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetPropertyExpanded, wParam, static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetPropertyExpanded, wParam, reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Retrieve a "property" value previously set with SetProperty,
	 * interpreted as an int AFTER any "$()" variable replacement.
	 */
	int32_t Editor::GetPropertyIntFromBuffer(Windows::Storage::Streams::IBuffer const &key, int32_t defaultValue)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetPropertyInt, reinterpret_cast<Scintilla::uptr_t>(key ? key.data() : nullptr), static_cast<Scintilla::sptr_t>(defaultValue)));
	}

	int32_t Editor::GetPropertyInt(hstring const &key, int32_t defaultValue)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetPropertyInt, reinterpret_cast<Scintilla::uptr_t>(to_string(key).c_str()), static_cast<Scintilla::sptr_t>(defaultValue)));
	}

	/*
	 * Retrieve the name of the lexer.
	 * Return the length of the text.
	 * Result is NUL-terminated.
	 */
	int32_t Editor::GetLexerLanguageWriteBuffer(Windows::Storage::Streams::IBuffer const &language)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetLexerLanguage, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(language ? language.data() : nullptr)));
	}

	hstring Editor::GetLexerLanguage()
	{
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetLexerLanguage, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetLexerLanguage, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * The starting style number for the sub styles associated with a base style
	 */
	int32_t Editor::GetSubStylesStart(int32_t styleBase)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetSubStylesStart, static_cast<Scintilla::uptr_t>(styleBase), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * The number of sub styles associated with a base style
	 */
	int32_t Editor::GetSubStylesLength(int32_t styleBase)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetSubStylesLength, static_cast<Scintilla::uptr_t>(styleBase), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * For a sub style, return the base style, else return the argument.
	 */
	int32_t Editor::GetStyleFromSubStyle(int32_t subStyle)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetStyleFromSubStyle, static_cast<Scintilla::uptr_t>(subStyle), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * For a secondary style, return the primary style, else return the argument.
	 */
	int32_t Editor::GetPrimaryStyleFromStyle(int32_t style)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetPrimaryStyleFromStyle, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(0)));
	}

	/*
	 * Get the set of base styles that can be extended with sub styles
	 * Result is NUL-terminated.
	 */
	int32_t Editor::GetSubStyleBasesWriteBuffer(Windows::Storage::Streams::IBuffer const &styles)
	{
		return static_cast<int32_t>(_scintilla->WndProc(Scintilla::Message::GetSubStyleBases, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(styles ? styles.data() : nullptr)));
	}

	hstring Editor::GetSubStyleBases()
	{
		const auto len{ static_cast<size_t>(_scintilla->WndProc(Scintilla::Message::GetSubStyleBases, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(0))) };
		if (len)
		{
			std::string value(len, '\0');
			_scintilla->WndProc(Scintilla::Message::GetSubStyleBases, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(value.data()));
			return to_hstring(value);
		}
		else
		{
			return hstring{};
		}
	}

	/*
	 * Set the locale for displaying text.
	 */
	void Editor::SetFontLocaleFromBuffer(Windows::Storage::Streams::IBuffer const &localeName)
	{
		_scintilla->WndProc(Scintilla::Message::SetFontLocale, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(localeName ? localeName.data() : nullptr));
	}

	void Editor::SetFontLocale(hstring const &localeName)
	{
		_scintilla->WndProc(Scintilla::Message::SetFontLocale, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(to_string(localeName).c_str()));
	}

	/*
	 * Set the foreground colour used for a particular marker number.
	 */
	void Editor::MarkerSetFore(int32_t markerNumber, int32_t fore)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerSetFore, static_cast<Scintilla::uptr_t>(markerNumber), static_cast<Scintilla::sptr_t>(fore));
	}

	/*
	 * Set the background colour used for a particular marker number.
	 */
	void Editor::MarkerSetBack(int32_t markerNumber, int32_t back)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerSetBack, static_cast<Scintilla::uptr_t>(markerNumber), static_cast<Scintilla::sptr_t>(back));
	}

	/*
	 * Set the background colour used for a particular marker number when its folding block is selected.
	 */
	void Editor::MarkerSetBackSelected(int32_t markerNumber, int32_t back)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerSetBackSelected, static_cast<Scintilla::uptr_t>(markerNumber), static_cast<Scintilla::sptr_t>(back));
	}

	/*
	 * Set the foreground colour used for a particular marker number.
	 */
	void Editor::MarkerSetForeTranslucent(int32_t markerNumber, int32_t fore)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerSetForeTranslucent, static_cast<Scintilla::uptr_t>(markerNumber), static_cast<Scintilla::sptr_t>(fore));
	}

	/*
	 * Set the background colour used for a particular marker number.
	 */
	void Editor::MarkerSetBackTranslucent(int32_t markerNumber, int32_t back)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerSetBackTranslucent, static_cast<Scintilla::uptr_t>(markerNumber), static_cast<Scintilla::sptr_t>(back));
	}

	/*
	 * Set the background colour used for a particular marker number when its folding block is selected.
	 */
	void Editor::MarkerSetBackSelectedTranslucent(int32_t markerNumber, int32_t back)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerSetBackSelectedTranslucent, static_cast<Scintilla::uptr_t>(markerNumber), static_cast<Scintilla::sptr_t>(back));
	}

	/*
	 * Set the width of strokes used in .01 pixels so 50  = 1/2 pixel width.
	 */
	void Editor::MarkerSetStrokeWidth(int32_t markerNumber, int32_t hundredths)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerSetStrokeWidth, static_cast<Scintilla::uptr_t>(markerNumber), static_cast<Scintilla::sptr_t>(hundredths));
	}

	/*
	 * Set the alpha used for a marker that is drawn in the text area, not the margin.
	 */
	void Editor::MarkerSetAlpha(int32_t markerNumber, MicaEditor::Alpha const &alpha)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerSetAlpha, static_cast<Scintilla::uptr_t>(markerNumber), static_cast<Scintilla::sptr_t>(alpha));
	}

	/*
	 * Set the layer used for a marker that is drawn in the text area, not the margin.
	 */
	void Editor::MarkerSetLayer(int32_t markerNumber, MicaEditor::Layer const &layer)
	{
		_scintilla->WndProc(Scintilla::Message::MarkerSetLayer, static_cast<Scintilla::uptr_t>(markerNumber), static_cast<Scintilla::sptr_t>(layer));
	}

	/*
	 * Set a margin to be either numeric or symbolic.
	 */
	void Editor::SetMarginTypeN(int32_t margin, MicaEditor::MarginType const &marginType)
	{
		_scintilla->WndProc(Scintilla::Message::SetMarginTypeN, static_cast<Scintilla::uptr_t>(margin), static_cast<Scintilla::sptr_t>(marginType));
	}

	/*
	 * Set the width of a margin to a width expressed in pixels.
	 */
	void Editor::SetMarginWidthN(int32_t margin, int32_t pixelWidth)
	{
		_scintilla->WndProc(Scintilla::Message::SetMarginWidthN, static_cast<Scintilla::uptr_t>(margin), static_cast<Scintilla::sptr_t>(pixelWidth));
	}

	/*
	 * Set a mask that determines which markers are displayed in a margin.
	 */
	void Editor::SetMarginMaskN(int32_t margin, int32_t mask)
	{
		_scintilla->WndProc(Scintilla::Message::SetMarginMaskN, static_cast<Scintilla::uptr_t>(margin), static_cast<Scintilla::sptr_t>(mask));
	}

	/*
	 * Make a margin sensitive or insensitive to mouse clicks.
	 */
	void Editor::SetMarginSensitiveN(int32_t margin, bool sensitive)
	{
		_scintilla->WndProc(Scintilla::Message::SetMarginSensitiveN, static_cast<Scintilla::uptr_t>(margin), static_cast<Scintilla::sptr_t>(sensitive));
	}

	/*
	 * Set the cursor shown when the mouse is inside a margin.
	 */
	void Editor::SetMarginCursorN(int32_t margin, MicaEditor::CursorShape const &cursor)
	{
		_scintilla->WndProc(Scintilla::Message::SetMarginCursorN, static_cast<Scintilla::uptr_t>(margin), static_cast<Scintilla::sptr_t>(cursor));
	}

	/*
	 * Set the background colour of a margin. Only visible for SC_MARGIN_COLOUR.
	 */
	void Editor::SetMarginBackN(int32_t margin, int32_t back)
	{
		_scintilla->WndProc(Scintilla::Message::SetMarginBackN, static_cast<Scintilla::uptr_t>(margin), static_cast<Scintilla::sptr_t>(back));
	}

	/*
	 * Set the foreground colour of a style.
	 */
	void Editor::StyleSetFore(int32_t style, int32_t fore)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetFore, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(fore));
	}

	/*
	 * Set the background colour of a style.
	 */
	void Editor::StyleSetBack(int32_t style, int32_t back)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetBack, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(back));
	}

	/*
	 * Set a style to be bold or not.
	 */
	void Editor::StyleSetBold(int32_t style, bool bold)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetBold, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(bold));
	}

	/*
	 * Set a style to be italic or not.
	 */
	void Editor::StyleSetItalic(int32_t style, bool italic)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetItalic, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(italic));
	}

	/*
	 * Set the size of characters of a style.
	 */
	void Editor::StyleSetSize(int32_t style, int32_t sizePoints)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetSize, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(sizePoints));
	}

	/*
	 * Set the font of a style.
	 */
	void Editor::StyleSetFontFromBuffer(int32_t style, Windows::Storage::Streams::IBuffer const &fontName)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetFont, static_cast<Scintilla::uptr_t>(style), reinterpret_cast<Scintilla::sptr_t>(fontName ? fontName.data() : nullptr));
	}

	void Editor::StyleSetFont(int32_t style, hstring const &fontName)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetFont, static_cast<Scintilla::uptr_t>(style), reinterpret_cast<Scintilla::sptr_t>(to_string(fontName).c_str()));
	}

	/*
	 * Set a style to have its end of line filled or not.
	 */
	void Editor::StyleSetEOLFilled(int32_t style, bool eolFilled)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetEOLFilled, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(eolFilled));
	}

	/*
	 * Set a style to be underlined or not.
	 */
	void Editor::StyleSetUnderline(int32_t style, bool underline)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetUnderline, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(underline));
	}

	/*
	 * Set a style to be mixed case, or to force upper or lower case.
	 */
	void Editor::StyleSetCase(int32_t style, MicaEditor::CaseVisible const &caseVisible)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetCase, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(caseVisible));
	}

	/*
	 * Set the size of characters of a style. Size is in points multiplied by 100.
	 */
	void Editor::StyleSetSizeFractional(int32_t style, int32_t sizeHundredthPoints)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetSizeFractional, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(sizeHundredthPoints));
	}

	/*
	 * Set the weight of characters of a style.
	 */
	void Editor::StyleSetWeight(int32_t style, MicaEditor::FontWeight const &weight)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetWeight, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(weight));
	}

	/*
	 * Set the character set of the font in a style.
	 */
	void Editor::StyleSetCharacterSet(int32_t style, MicaEditor::CharacterSet const &characterSet)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetCharacterSet, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(characterSet));
	}

	/*
	 * Set a style to be a hotspot or not.
	 */
	void Editor::StyleSetHotSpot(int32_t style, bool hotspot)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetHotSpot, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(hotspot));
	}

	/*
	 * Indicate that a style may be monospaced over ASCII graphics characters which enables optimizations.
	 */
	void Editor::StyleSetCheckMonospaced(int32_t style, bool checkMonospaced)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetCheckMonospaced, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(checkMonospaced));
	}

	/*
	 * Set the invisible representation for a style.
	 */
	void Editor::StyleSetInvisibleRepresentationFromBuffer(int32_t style, Windows::Storage::Streams::IBuffer const &representation)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetInvisibleRepresentation, static_cast<Scintilla::uptr_t>(style), reinterpret_cast<Scintilla::sptr_t>(representation ? representation.data() : nullptr));
	}

	void Editor::StyleSetInvisibleRepresentation(int32_t style, hstring const &representation)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetInvisibleRepresentation, static_cast<Scintilla::uptr_t>(style), reinterpret_cast<Scintilla::sptr_t>(to_string(representation).c_str()));
	}

	/*
	 * Set the colour of an element. Translucency (alpha) may or may not be significant
	 * and this may depend on the platform. The alpha byte should commonly be 0xff for opaque.
	 */
	void Editor::SetElementColour(MicaEditor::Element const &element, int32_t colourElement)
	{
		_scintilla->WndProc(Scintilla::Message::SetElementColour, static_cast<Scintilla::uptr_t>(element), static_cast<Scintilla::sptr_t>(colourElement));
	}

	/*
	 * Set a style to be visible or not.
	 */
	void Editor::StyleSetVisible(int32_t style, bool visible)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetVisible, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(visible));
	}

	/*
	 * Set the set of characters making up words for when moving or selecting by word.
	 * First sets defaults like SetCharsDefault.
	 */
	void Editor::SetWordCharsFromBuffer(Windows::Storage::Streams::IBuffer const &characters)
	{
		_scintilla->WndProc(Scintilla::Message::SetWordChars, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(characters ? characters.data() : nullptr));
	}

	void Editor::SetWordChars(hstring const &characters)
	{
		_scintilla->WndProc(Scintilla::Message::SetWordChars, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(to_string(characters).c_str()));
	}

	/*
	 * Set an indicator to plain, squiggle or TT.
	 */
	void Editor::IndicSetStyle(int32_t indicator, MicaEditor::IndicatorStyle const &indicatorStyle)
	{
		_scintilla->WndProc(Scintilla::Message::IndicSetStyle, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(indicatorStyle));
	}

	/*
	 * Set the foreground colour of an indicator.
	 */
	void Editor::IndicSetFore(int32_t indicator, int32_t fore)
	{
		_scintilla->WndProc(Scintilla::Message::IndicSetFore, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(fore));
	}

	/*
	 * Set an indicator to draw under text or over(default).
	 */
	void Editor::IndicSetUnder(int32_t indicator, bool under)
	{
		_scintilla->WndProc(Scintilla::Message::IndicSetUnder, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(under));
	}

	/*
	 * Set a hover indicator to plain, squiggle or TT.
	 */
	void Editor::IndicSetHoverStyle(int32_t indicator, MicaEditor::IndicatorStyle const &indicatorStyle)
	{
		_scintilla->WndProc(Scintilla::Message::IndicSetHoverStyle, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(indicatorStyle));
	}

	/*
	 * Set the foreground hover colour of an indicator.
	 */
	void Editor::IndicSetHoverFore(int32_t indicator, int32_t fore)
	{
		_scintilla->WndProc(Scintilla::Message::IndicSetHoverFore, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(fore));
	}

	/*
	 * Set the attributes of an indicator.
	 */
	void Editor::IndicSetFlags(int32_t indicator, MicaEditor::IndicFlag const &flags)
	{
		_scintilla->WndProc(Scintilla::Message::IndicSetFlags, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(flags));
	}

	/*
	 * Set the stroke width of an indicator in hundredths of a pixel.
	 */
	void Editor::IndicSetStrokeWidth(int32_t indicator, int32_t hundredths)
	{
		_scintilla->WndProc(Scintilla::Message::IndicSetStrokeWidth, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(hundredths));
	}

	/*
	 * Used to hold extra styling information for each line.
	 */
	void Editor::SetLineState(int64_t line, int32_t state)
	{
		_scintilla->WndProc(Scintilla::Message::SetLineState, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(state));
	}

	/*
	 * Set a style to be changeable or not (read only).
	 * Experimental feature, currently buggy.
	 */
	void Editor::StyleSetChangeable(int32_t style, bool changeable)
	{
		_scintilla->WndProc(Scintilla::Message::StyleSetChangeable, static_cast<Scintilla::uptr_t>(style), static_cast<Scintilla::sptr_t>(changeable));
	}

	/*
	 * Define a set of characters that when typed will cause the autocompletion to
	 * choose the selected item.
	 */
	void Editor::AutoCSetFillUpsFromBuffer(Windows::Storage::Streams::IBuffer const &characterSet)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSetFillUps, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(characterSet ? characterSet.data() : nullptr));
	}

	void Editor::AutoCSetFillUps(hstring const &characterSet)
	{
		_scintilla->WndProc(Scintilla::Message::AutoCSetFillUps, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(to_string(characterSet).c_str()));
	}

	/*
	 * Change the indentation of a line to a number of columns.
	 */
	void Editor::SetLineIndentation(int64_t line, int32_t indentation)
	{
		_scintilla->WndProc(Scintilla::Message::SetLineIndentation, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(indentation));
	}

	/*
	 * Enlarge the number of lines allocated.
	 */
	void Editor::AllocateLines(int64_t lines)
	{
		_scintilla->WndProc(Scintilla::Message::AllocateLines, static_cast<Scintilla::uptr_t>(lines), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the start position in order to change when backspacing removes the calltip.
	 */
	void Editor::CallTipSetPosStart(int64_t posStart)
	{
		_scintilla->WndProc(Scintilla::Message::CallTipSetPosStart, static_cast<Scintilla::uptr_t>(posStart), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the background colour for the call tip.
	 */
	void Editor::CallTipSetBack(int32_t back)
	{
		_scintilla->WndProc(Scintilla::Message::CallTipSetBack, static_cast<Scintilla::uptr_t>(back), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the foreground colour for the call tip.
	 */
	void Editor::CallTipSetFore(int32_t fore)
	{
		_scintilla->WndProc(Scintilla::Message::CallTipSetFore, static_cast<Scintilla::uptr_t>(fore), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the foreground colour for the highlighted part of the call tip.
	 */
	void Editor::CallTipSetForeHlt(int32_t fore)
	{
		_scintilla->WndProc(Scintilla::Message::CallTipSetForeHlt, static_cast<Scintilla::uptr_t>(fore), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Enable use of STYLE_CALLTIP and set call tip tab size in pixels.
	 */
	void Editor::CallTipUseStyle(int32_t tabSize)
	{
		_scintilla->WndProc(Scintilla::Message::CallTipUseStyle, static_cast<Scintilla::uptr_t>(tabSize), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set position of calltip, above or below text.
	 */
	void Editor::CallTipSetPosition(bool above)
	{
		_scintilla->WndProc(Scintilla::Message::CallTipSetPosition, static_cast<Scintilla::uptr_t>(above), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the fold level of a line.
	 * This encodes an integer level along with flags indicating whether the
	 * line is a header and whether it is effectively white space.
	 */
	void Editor::SetFoldLevel(int64_t line, MicaEditor::FoldLevel const &level)
	{
		_scintilla->WndProc(Scintilla::Message::SetFoldLevel, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(level));
	}

	/*
	 * Show the children of a header line.
	 */
	void Editor::SetFoldExpanded(int64_t line, bool expanded)
	{
		_scintilla->WndProc(Scintilla::Message::SetFoldExpanded, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(expanded));
	}

	/*
	 * Set some style options for folding.
	 */
	void Editor::SetFoldFlags(MicaEditor::FoldFlag const &flags)
	{
		_scintilla->WndProc(Scintilla::Message::SetFoldFlags, static_cast<Scintilla::uptr_t>(flags), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set a fore colour for active hotspots.
	 */
	void Editor::SetHotspotActiveFore(bool useSetting, int32_t fore)
	{
		_scintilla->WndProc(Scintilla::Message::SetHotspotActiveFore, static_cast<Scintilla::uptr_t>(useSetting), static_cast<Scintilla::sptr_t>(fore));
	}

	/*
	 * Set a back colour for active hotspots.
	 */
	void Editor::SetHotspotActiveBack(bool useSetting, int32_t back)
	{
		_scintilla->WndProc(Scintilla::Message::SetHotspotActiveBack, static_cast<Scintilla::uptr_t>(useSetting), static_cast<Scintilla::sptr_t>(back));
	}

	/*
	 * Set the set of characters making up whitespace for when moving or selecting by word.
	 * Should be called after SetWordChars.
	 */
	void Editor::SetWhitespaceCharsFromBuffer(Windows::Storage::Streams::IBuffer const &characters)
	{
		_scintilla->WndProc(Scintilla::Message::SetWhitespaceChars, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(characters ? characters.data() : nullptr));
	}

	void Editor::SetWhitespaceChars(hstring const &characters)
	{
		_scintilla->WndProc(Scintilla::Message::SetWhitespaceChars, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(to_string(characters).c_str()));
	}

	/*
	 * Set the set of characters making up punctuation characters
	 * Should be called after SetWordChars.
	 */
	void Editor::SetPunctuationCharsFromBuffer(Windows::Storage::Streams::IBuffer const &characters)
	{
		_scintilla->WndProc(Scintilla::Message::SetPunctuationChars, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(characters ? characters.data() : nullptr));
	}

	void Editor::SetPunctuationChars(hstring const &characters)
	{
		_scintilla->WndProc(Scintilla::Message::SetPunctuationChars, static_cast<Scintilla::uptr_t>(0), reinterpret_cast<Scintilla::sptr_t>(to_string(characters).c_str()));
	}

	/*
	 * Set the alpha fill colour of the given indicator.
	 */
	void Editor::IndicSetAlpha(int32_t indicator, MicaEditor::Alpha const &alpha)
	{
		_scintilla->WndProc(Scintilla::Message::IndicSetAlpha, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(alpha));
	}

	/*
	 * Set the alpha outline colour of the given indicator.
	 */
	void Editor::IndicSetOutlineAlpha(int32_t indicator, MicaEditor::Alpha const &alpha)
	{
		_scintilla->WndProc(Scintilla::Message::IndicSetOutlineAlpha, static_cast<Scintilla::uptr_t>(indicator), static_cast<Scintilla::sptr_t>(alpha));
	}

	/*
	 * Set the text in the text margin for a line
	 */
	void Editor::MarginSetTextFromBuffer(int64_t line, Windows::Storage::Streams::IBuffer const &text)
	{
		_scintilla->WndProc(Scintilla::Message::MarginSetText, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr));
	}

	void Editor::MarginSetText(int64_t line, hstring const &text)
	{
		_scintilla->WndProc(Scintilla::Message::MarginSetText, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str()));
	}

	/*
	 * Set the style number for the text margin for a line
	 */
	void Editor::MarginSetStyle(int64_t line, int32_t style)
	{
		_scintilla->WndProc(Scintilla::Message::MarginSetStyle, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(style));
	}

	/*
	 * Set the style in the text margin for a line
	 */
	void Editor::MarginSetStylesFromBuffer(int64_t line, Windows::Storage::Streams::IBuffer const &styles)
	{
		_scintilla->WndProc(Scintilla::Message::MarginSetStyles, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(styles ? styles.data() : nullptr));
	}

	void Editor::MarginSetStyles(int64_t line, hstring const &styles)
	{
		_scintilla->WndProc(Scintilla::Message::MarginSetStyles, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(to_string(styles).c_str()));
	}

	/*
	 * Set the annotation text for a line
	 */
	void Editor::AnnotationSetTextFromBuffer(int64_t line, Windows::Storage::Streams::IBuffer const &text)
	{
		_scintilla->WndProc(Scintilla::Message::AnnotationSetText, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr));
	}

	void Editor::AnnotationSetText(int64_t line, hstring const &text)
	{
		_scintilla->WndProc(Scintilla::Message::AnnotationSetText, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str()));
	}

	/*
	 * Set the style number for the annotations for a line
	 */
	void Editor::AnnotationSetStyle(int64_t line, int32_t style)
	{
		_scintilla->WndProc(Scintilla::Message::AnnotationSetStyle, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(style));
	}

	/*
	 * Set the annotation styles for a line
	 */
	void Editor::AnnotationSetStylesFromBuffer(int64_t line, Windows::Storage::Streams::IBuffer const &styles)
	{
		_scintilla->WndProc(Scintilla::Message::AnnotationSetStyles, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(styles ? styles.data() : nullptr));
	}

	void Editor::AnnotationSetStyles(int64_t line, hstring const &styles)
	{
		_scintilla->WndProc(Scintilla::Message::AnnotationSetStyles, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(to_string(styles).c_str()));
	}

	/*
	 * Set the caret position of the nth selection.
	 */
	void Editor::SetSelectionNCaret(int32_t selection, int64_t caret)
	{
		_scintilla->WndProc(Scintilla::Message::SetSelectionNCaret, static_cast<Scintilla::uptr_t>(selection), static_cast<Scintilla::sptr_t>(caret));
	}

	/*
	 * Set the anchor position of the nth selection.
	 */
	void Editor::SetSelectionNAnchor(int32_t selection, int64_t anchor)
	{
		_scintilla->WndProc(Scintilla::Message::SetSelectionNAnchor, static_cast<Scintilla::uptr_t>(selection), static_cast<Scintilla::sptr_t>(anchor));
	}

	/*
	 * Set the virtual space of the caret of the nth selection.
	 */
	void Editor::SetSelectionNCaretVirtualSpace(int32_t selection, int64_t space)
	{
		_scintilla->WndProc(Scintilla::Message::SetSelectionNCaretVirtualSpace, static_cast<Scintilla::uptr_t>(selection), static_cast<Scintilla::sptr_t>(space));
	}

	/*
	 * Set the virtual space of the anchor of the nth selection.
	 */
	void Editor::SetSelectionNAnchorVirtualSpace(int32_t selection, int64_t space)
	{
		_scintilla->WndProc(Scintilla::Message::SetSelectionNAnchorVirtualSpace, static_cast<Scintilla::uptr_t>(selection), static_cast<Scintilla::sptr_t>(space));
	}

	/*
	 * Sets the position that starts the selection - this becomes the anchor.
	 */
	void Editor::SetSelectionNStart(int32_t selection, int64_t anchor)
	{
		_scintilla->WndProc(Scintilla::Message::SetSelectionNStart, static_cast<Scintilla::uptr_t>(selection), static_cast<Scintilla::sptr_t>(anchor));
	}

	/*
	 * Sets the position that ends the selection - this becomes the currentPosition.
	 */
	void Editor::SetSelectionNEnd(int32_t selection, int64_t caret)
	{
		_scintilla->WndProc(Scintilla::Message::SetSelectionNEnd, static_cast<Scintilla::uptr_t>(selection), static_cast<Scintilla::sptr_t>(caret));
	}

	/*
	 * Set the foreground colour of additional selections.
	 * Must have previously called SetSelFore with non-zero first argument for this to have an effect.
	 */
	void Editor::SetAdditionalSelFore(int32_t fore)
	{
		_scintilla->WndProc(Scintilla::Message::SetAdditionalSelFore, static_cast<Scintilla::uptr_t>(fore), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the background colour of additional selections.
	 * Must have previously called SetSelBack with non-zero first argument for this to have an effect.
	 */
	void Editor::SetAdditionalSelBack(int32_t back)
	{
		_scintilla->WndProc(Scintilla::Message::SetAdditionalSelBack, static_cast<Scintilla::uptr_t>(back), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the width for future RGBA image data.
	 */
	void Editor::RGBAImageSetWidth(int32_t width)
	{
		_scintilla->WndProc(Scintilla::Message::RGBAImageSetWidth, static_cast<Scintilla::uptr_t>(width), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the height for future RGBA image data.
	 */
	void Editor::RGBAImageSetHeight(int32_t height)
	{
		_scintilla->WndProc(Scintilla::Message::RGBAImageSetHeight, static_cast<Scintilla::uptr_t>(height), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the scale factor in percent for future RGBA image data.
	 */
	void Editor::RGBAImageSetScale(int32_t scalePercent)
	{
		_scintilla->WndProc(Scintilla::Message::RGBAImageSetScale, static_cast<Scintilla::uptr_t>(scalePercent), static_cast<Scintilla::sptr_t>(0));
	}

	/*
	 * Set the way a character is drawn.
	 */
	void Editor::SetRepresentationFromBuffer(Windows::Storage::Streams::IBuffer const &encodedCharacter, Windows::Storage::Streams::IBuffer const &representation)
	{
		_scintilla->WndProc(Scintilla::Message::SetRepresentation, reinterpret_cast<Scintilla::uptr_t>(encodedCharacter ? encodedCharacter.data() : nullptr), reinterpret_cast<Scintilla::sptr_t>(representation ? representation.data() : nullptr));
	}

	void Editor::SetRepresentation(hstring const &encodedCharacter, hstring const &representation)
	{
		_scintilla->WndProc(Scintilla::Message::SetRepresentation, reinterpret_cast<Scintilla::uptr_t>(to_string(encodedCharacter).c_str()), reinterpret_cast<Scintilla::sptr_t>(to_string(representation).c_str()));
	}

	/*
	 * Set the appearance of a representation.
	 */
	void Editor::SetRepresentationAppearanceFromBuffer(Windows::Storage::Streams::IBuffer const &encodedCharacter, MicaEditor::RepresentationAppearance const &appearance)
	{
		_scintilla->WndProc(Scintilla::Message::SetRepresentationAppearance, reinterpret_cast<Scintilla::uptr_t>(encodedCharacter ? encodedCharacter.data() : nullptr), static_cast<Scintilla::sptr_t>(appearance));
	}

	void Editor::SetRepresentationAppearance(hstring const &encodedCharacter, MicaEditor::RepresentationAppearance const &appearance)
	{
		_scintilla->WndProc(Scintilla::Message::SetRepresentationAppearance, reinterpret_cast<Scintilla::uptr_t>(to_string(encodedCharacter).c_str()), static_cast<Scintilla::sptr_t>(appearance));
	}

	/*
	 * Set the colour of a representation.
	 */
	void Editor::SetRepresentationColourFromBuffer(Windows::Storage::Streams::IBuffer const &encodedCharacter, int32_t colour)
	{
		_scintilla->WndProc(Scintilla::Message::SetRepresentationColour, reinterpret_cast<Scintilla::uptr_t>(encodedCharacter ? encodedCharacter.data() : nullptr), static_cast<Scintilla::sptr_t>(colour));
	}

	void Editor::SetRepresentationColour(hstring const &encodedCharacter, int32_t colour)
	{
		_scintilla->WndProc(Scintilla::Message::SetRepresentationColour, reinterpret_cast<Scintilla::uptr_t>(to_string(encodedCharacter).c_str()), static_cast<Scintilla::sptr_t>(colour));
	}

	/*
	 * Set the end of line annotation text for a line
	 */
	void Editor::EOLAnnotationSetTextFromBuffer(int64_t line, Windows::Storage::Streams::IBuffer const &text)
	{
		_scintilla->WndProc(Scintilla::Message::EOLAnnotationSetText, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(text ? text.data() : nullptr));
	}

	void Editor::EOLAnnotationSetText(int64_t line, hstring const &text)
	{
		_scintilla->WndProc(Scintilla::Message::EOLAnnotationSetText, static_cast<Scintilla::uptr_t>(line), reinterpret_cast<Scintilla::sptr_t>(to_string(text).c_str()));
	}

	/*
	 * Set the style number for the end of line annotations for a line
	 */
	void Editor::EOLAnnotationSetStyle(int64_t line, int32_t style)
	{
		_scintilla->WndProc(Scintilla::Message::EOLAnnotationSetStyle, static_cast<Scintilla::uptr_t>(line), static_cast<Scintilla::sptr_t>(style));
	}

	/*
	 * Set up a value that may be used by a lexer for some optional feature.
	 */
	void Editor::SetPropertyFromBuffer(Windows::Storage::Streams::IBuffer const &key, Windows::Storage::Streams::IBuffer const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetProperty, reinterpret_cast<Scintilla::uptr_t>(key ? key.data() : nullptr), reinterpret_cast<Scintilla::sptr_t>(value ? value.data() : nullptr));
	}

	void Editor::SetProperty(hstring const &key, hstring const &value)
	{
		_scintilla->WndProc(Scintilla::Message::SetProperty, reinterpret_cast<Scintilla::uptr_t>(to_string(key).c_str()), reinterpret_cast<Scintilla::sptr_t>(to_string(value).c_str()));
	}

	/*
	 * Set up the key words used by the lexer.
	 */
	void Editor::SetKeyWordsFromBuffer(int32_t keyWordSet, Windows::Storage::Streams::IBuffer const &keyWords)
	{
		_scintilla->WndProc(Scintilla::Message::SetKeyWords, static_cast<Scintilla::uptr_t>(keyWordSet), reinterpret_cast<Scintilla::sptr_t>(keyWords ? keyWords.data() : nullptr));
	}

	void Editor::SetKeyWords(int32_t keyWordSet, hstring const &keyWords)
	{
		_scintilla->WndProc(Scintilla::Message::SetKeyWords, static_cast<Scintilla::uptr_t>(keyWordSet), reinterpret_cast<Scintilla::sptr_t>(to_string(keyWords).c_str()));
	}

	/*
	 * Set the identifiers that are shown in a particular style
	 */
	void Editor::SetIdentifiersFromBuffer(int32_t style, Windows::Storage::Streams::IBuffer const &identifiers)
	{
		_scintilla->WndProc(Scintilla::Message::SetIdentifiers, static_cast<Scintilla::uptr_t>(style), reinterpret_cast<Scintilla::sptr_t>(identifiers ? identifiers.data() : nullptr));
	}

	void Editor::SetIdentifiers(int32_t style, hstring const &identifiers)
	{
		_scintilla->WndProc(Scintilla::Message::SetIdentifiers, static_cast<Scintilla::uptr_t>(style), reinterpret_cast<Scintilla::sptr_t>(to_string(identifiers).c_str()));
	}

	/*
	 * Set the lexer from an ILexer*.
	 */
	void Editor::SetILexer(uint64_t ilexer)
	{
		_scintilla->WndProc(Scintilla::Message::SetILexer, static_cast<Scintilla::uptr_t>(0), static_cast<Scintilla::sptr_t>(ilexer));
	}
}
