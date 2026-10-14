#include "Models/Head/editor.h"
#include "Models/Head/executableobjects.h"
#include "Models/Head/mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_editor.h"
#include <QPushButton>
#include <QToolButton>
#include "Models/Head/notify.h"
#include <TFile.h>
#include <TKey.h>
#include <TROOT.h>
#include <TH1.h>
#include <TAxis.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TGraphBentErrors.h>
#include <TGraphPolar.h>
#include <TGraph2D.h>
#include <TGraphQQ.h>
#include <TBox.h>
#include <TEllipse.h>
#include <TArc.h>
#include <TCurlyLine.h>
#include <TCrown.h>
#include <TCanvas.h>
#include <TAttLine.h>
#include <TAttFill.h>
#include <TAttMarker.h>
#include <TAttText.h>
#include <TFrame.h>
#include <TColor.h>
#include <TF1.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TDirectory.h>
#include <TClass.h>
#include <TMethod.h>
#include <TMethodArg.h>
#include <TMethodCall.h>
#include <TNamed.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TPaveStats.h>
#include <TPaveLabel.h>
#include <TText.h>
#include <TObject.h>
#include <TLegend.h>
#include <QFont>
#include <QColor>
#include <QTimer>
#include <QLayout>
#include <QProcess>
#include <QFileInfo>
#include <QFile>
#include <sstream>
#include <QSet>
#include <QStringList>
#include <memory>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstring>
#include "Models/Lib/flowlayout.h"
#include "Models/Lib/QAdvanceddocking/DockManager.h"
#include "Models/Lib/QAdvanceddocking/DockAreaWidget.h"
#include "Models/Lib/QPropertybrowser/qtpropertymanager.h"
#include "Models/Lib/QPropertybrowser/qtvariantproperty.h"
#include "Models/Lib/QPropertybrowser/qttreepropertybrowser.h"
#include <QUrl>
#include <QUrlQuery>
#include <QRegularExpression>
#include "TMultiGraph.h"

#define COLREF(r, g, b) ((long)((r) | ((g) << 8) | ((b) << 16)))

namespace {

enum StyleId {
	STY_DEFAULT = 0,
	STY_KEYWORD,
	STY_TYPE,
	STY_COMMENT,
	STY_COMMENTLINE,
	STY_STRING,
	STY_CHAR,
	STY_NUMBER,
	STY_PREPROC,
	STY_OPERATOR,
};

constexpr int kDefaultStyle = 32;

bool isIdentStart(unsigned char c) {
	return std::isalpha(c) || c == '_';
}

bool isIdentChar(unsigned char c) {
	return std::isalnum(c) || c == '_';
}

bool isBracketChar(char c) {
	return c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}';
}

bool isOperatorChar(unsigned char c) {
	return std::strchr("+-*/%=<>!&|^~?:;,.()[]{}", c) != nullptr;
}

const QSet<QString> &cppKeywords() {
	static const QSet<QString> kw = {
		"alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor",
		"bool", "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t",
		"class", "compl", "concept", "const", "consteval", "constexpr", "constinit",
		"const_cast", "continue", "co_await", "co_return", "co_yield", "decltype",
		"default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit",
		"export", "extern", "false", "float", "for", "friend", "goto", "if", "inline",
		"int", "long", "mutable", "namespace", "new", "noexcept", "not", "not_eq",
		"nullptr", "operator", "or", "or_eq", "private", "protected", "public",
		"register", "reinterpret_cast", "requires", "return", "short", "signed",
		"sizeof", "static", "static_assert", "static_cast", "struct", "switch",
		"template", "this", "thread_local", "throw", "true", "try", "typedef",
		"typeid", "typename", "union", "unsigned", "using", "virtual", "void",
		"volatile", "wchar_t", "while", "xor", "xor_eq",
	};
	return kw;
}

const QSet<QString> &cppTypes() {
	static const QSet<QString> tp = {
		"size_t", "ssize_t", "ptrdiff_t", "intptr_t", "uintptr_t",
		"int8_t", "int16_t", "int32_t", "int64_t",
		"uint8_t", "uint16_t", "uint32_t", "uint64_t",
		"std", "string", "wstring", "u8string", "vector", "array", "deque", "list",
		"map", "multimap", "set", "multiset", "unordered_map", "unordered_set",
		"pair", "tuple", "shared_ptr", "unique_ptr", "weak_ptr", "optional",
		"variant", "function", "iostream", "istream", "ostream", "sstream",
		"stringstream", "ifstream", "ofstream", "fstream", "thread", "mutex",
	};
	return tp;
}

const QSet<QString> &rootKeywords() {
	static const QSet<QString> kw = {
		"TH1F", "TH1D", "TH2F", "TGraph", "TCanvas", "TFile", "TTree", "TF1",
		"gROOT", "gPad", "SetBinContent", "SetEntries", "SetLineColor",
		"SetLineWidth", "SetMarkerStyle", "Draw", "GetXaxis", "GetYaxis",
		"GetZaxis", "SetLabelFont", "SetLabelSize", "SetTitleSize",
		"SetTitleOffset", "SetTitleFont", "Open", "Get", "Close",
		"SavePrimitive",
	};
	return kw;
}

QByteArray CppCompletionList() {
	QStringList words;
	for (const QString &w : cppKeywords())
		words << w;
	for (const QString &w : cppTypes())
		words << w;
	for (const QString &w : rootKeywords())
		words << w;
	words.sort();
	words.removeDuplicates();
	return words.join('?').toLatin1();
}

QByteArray styleCpp(const QByteArray &text) {
	const int n = text.size();
	QByteArray styles(n, static_cast<char>(STY_DEFAULT));
	if (n <= 0)
		return styles;
	const char *p = text.constData();

	for (int i = 0; i < n;) {
		const unsigned char c = static_cast<unsigned char>(p[i]);

		if (c == '/' && i + 1 < n && p[i + 1] == '/') {
			int j = i;
			while (j < n && p[j] != '\n' && p[j] != '\r') j++;
			std::fill(styles.begin() + i, styles.begin() + j, STY_COMMENTLINE);
			i = j;
			continue;
		}
		if (c == '/' && i + 1 < n && p[i + 1] == '*') {
			int j = i + 2;
			while (j < n && !(p[j] == '*' && j + 1 < n && p[j + 1] == '/')) j++;
			j = (j + 1 < n) ? j + 2 : n;
			std::fill(styles.begin() + i, styles.begin() + j, STY_COMMENT);
			i = j;
			continue;
		}
		if (c == '"') {
			int j = i + 1;
			while (j < n && p[j] != '"') {
				if (p[j] == '\\') j++;
				j++;
			}
			if (j < n) j++;
			std::fill(styles.begin() + i, styles.begin() + j, STY_STRING);
			i = j;
			continue;
		}
		if (c == '\'') {
			int j = i + 1;
			while (j < n && p[j] != '\'') {
				if (p[j] == '\\') j++;
				j++;
			}
			if (j < n) j++;
			std::fill(styles.begin() + i, styles.begin() + j, STY_CHAR);
			i = j;
			continue;
		}
		if (c == '#') {
			int k = i - 1;
			bool lineStart = true;
			while (k >= 0 && p[k] != '\n' && p[k] != '\r') {
				if (p[k] != ' ' && p[k] != '\t') {
					lineStart = false;
					break;
				}
				k--;
			}
			if (lineStart) {
				int j = i;
				while (j < n && p[j] != '\n' && p[j] != '\r') j++;
				std::fill(styles.begin() + i, styles.begin() + j, STY_PREPROC);
				i = j;
				continue;
			}
		}
		if (isIdentStart(c)) {
			int j = i;
			while (j < n && isIdentChar(static_cast<unsigned char>(p[j]))) j++;
			const QString word = QString::fromLatin1(p + i, j - i);
			char st = STY_DEFAULT;
			if (cppKeywords().contains(word) || rootKeywords().contains(word))
				st = STY_KEYWORD;
			else if (cppTypes().contains(word))
				st = STY_TYPE;
			std::fill(styles.begin() + i, styles.begin() + j, st);
			i = j;
			continue;
		}
		if (std::isdigit(c)) {
			int j = i;
			while (j < n && (isIdentChar(static_cast<unsigned char>(p[j])) ||
			                 p[j] == '.' || p[j] == '\'')) j++;
			std::fill(styles.begin() + i, styles.begin() + j, STY_NUMBER);
			i = j;
			continue;
		}
		if (isOperatorChar(c)) {
			styles[i] = STY_OPERATOR;
			i++;
			continue;
		}
		i++;
	}
	return styles;
}

static void autoClosePairStatic(ScintillaEdit *edit, char ch) {
	const long pos = edit->currentPos();
	if (pos > 0) {
		const int style = static_cast<int>(edit->styleAt(pos - 1));
		if (style == STY_STRING || style == STY_CHAR || style == STY_COMMENT ||
		    style == STY_COMMENTLINE)
			return;
	}

	switch (ch) {
	case '(':
		if (static_cast<char>(edit->charAt(pos)) != ')') {
			edit->insertText(pos, ")");
			edit->setSel(pos, pos);
		}
		break;
	case '[':
		if (static_cast<char>(edit->charAt(pos)) != ']') {
			edit->insertText(pos, "]");
			edit->setSel(pos, pos);
		}
		break;
	case '{':
		if (static_cast<char>(edit->charAt(pos)) != '}') {
			edit->insertText(pos, "}");
			edit->setSel(pos, pos);
		}
		break;
	case ')':
		if (static_cast<char>(edit->charAt(pos)) == ')')
			edit->deleteRange(pos, 1);
		break;
	case ']':
		if (static_cast<char>(edit->charAt(pos)) == ']')
			edit->deleteRange(pos, 1);
		break;
	case '}':
		if (static_cast<char>(edit->charAt(pos)) == '}')
			edit->deleteRange(pos, 1);
		break;
	case '"':
	case '\'':
		if (pos > 1 && pos <= edit->textLength() &&
		    isIdentChar(static_cast<unsigned char>(edit->charAt(pos - 2)))) {
			if (static_cast<char>(edit->charAt(pos)) == ch)
				edit->deleteRange(pos, 1);
			break;
		}
		if (static_cast<char>(edit->charAt(pos)) != ch) {
			QByteArray cls(1, ch);
			edit->insertText(pos, cls);
			edit->setSel(pos, pos);
		}
		break;
	default:
		break;
	}
}

static void autoIndentBraceBody(ScintillaEdit *edit, bool &inEditAction) {
	const long pos = edit->currentPos();
	if (pos < 1 || pos > edit->textLength())
		return;
	if (static_cast<char>(edit->charAt(pos)) != '}')
		return;

	const long curLine = edit->lineFromPosition(pos);
	if (curLine < 1)
		return;
	const long prevLine = curLine - 1;
	const long prevLineEnd = edit->lineEndPosition(prevLine);
	long j = prevLineEnd - 1;
	while (j >= 0 && (static_cast<char>(edit->charAt(j)) == ' ' ||
	                  static_cast<char>(edit->charAt(j)) == '\t'))
		j--;
	if (j < 0 || static_cast<char>(edit->charAt(j)) != '{')
		return;

	const long prevIndent = edit->lineIndentation(prevLine);
	const long line2Start = edit->positionFromLine(curLine);
	const long line2End = edit->lineEndPosition(curLine);
	const long bodyIndent = prevIndent + edit->tabWidth();

	QString repl;
	repl.append(QString(static_cast<int>(bodyIndent), ' '));
	repl.append('\n');
	repl.append(QString(static_cast<int>(prevIndent), ' '));
	repl.append('}');

	inEditAction = true;
	edit->beginUndoAction();
	edit->deleteRange(line2Start, line2End - line2Start);
	edit->insertText(line2Start, repl.toLatin1());
	const long caret = line2Start + bodyIndent;
	edit->setSel(caret, caret);
	edit->endUndoAction();
	inEditAction = false;
}

static void maybeShowCompletion(ScintillaEdit *edit) {
	if (edit->autoCActive())
		return;
	const long pos = edit->currentPos();
	if (pos < 2)
		return;
	const long wordStart = edit->wordStartPosition(pos, true);
	const long wlen = pos - wordStart;
	if (wlen < 2)
		return;
	if (std::isdigit(static_cast<unsigned char>(edit->charAt(wordStart))))
		return;
	const char before = static_cast<char>(edit->charAt(wordStart - 1));
	if (before == '.' || before == ':' || before == '>' || before == '-' || before == '/')
		return;
	const int style = static_cast<int>(edit->styleAt(wordStart));
	if (style == STY_STRING || style == STY_CHAR || style == STY_COMMENT ||
	    style == STY_COMMENTLINE)
		return;

	static const QByteArray items = CppCompletionList();
	edit->autoCShow(wlen, items.constData());
}

} // anonymous namespace

bool Editor::canEditObject(TObject *obj) {
	if (!obj)
		return false;
	return obj->InheritsFrom("TH1") || obj->InheritsFrom("TGraph") ||
	       obj->InheritsFrom("TF1") || obj->InheritsFrom("TTree") ||
	       obj->InheritsFrom("TCanvas") || obj->InheritsFrom("TBox") ||
	       obj->InheritsFrom("TEllipse") || obj->InheritsFrom("TText");
}

void Editor::setupCodeEditor(TObject *robj) {
	qDebug() << "[CODE] setupCodeEditor begin";
	if (!robj) return;
	if (codeEditor) {
		ui->verticalLayout_9->removeWidget(codeEditor);
		codeEditor->deleteLater();
		codeEditor = nullptr;
	}

	codeEditor = new ScintillaEdit(this);
	codeEditor->setCodePage(65001);

	codeEditor->setTabWidth(4);
	codeEditor->setUseTabs(false);
	codeEditor->setCaretLineVisible(true);
	codeEditor->setCaretLineBack(0xF0F0F0);

	codeEditor->styleSetFont(kDefaultStyle, "Consolas");
	codeEditor->styleSetSize(kDefaultStyle, 11);
	codeEditor->styleClearAll();
	auto setStyle = [this](int style, long fore, bool bold, bool italic) {
		codeEditor->styleSetFore(style, fore);
		codeEditor->styleSetBold(style, bold);
		codeEditor->styleSetItalic(style, italic);
	};
	setStyle(STY_DEFAULT,    COLREF(0x00, 0x00, 0x00), false, false);
	setStyle(STY_KEYWORD,    COLREF(0x00, 0x00, 0xFF), true,  false);
	setStyle(STY_TYPE,       COLREF(0x2B, 0x91, 0xAF), false, false);
	setStyle(STY_COMMENT,    COLREF(0x00, 0x80, 0x00), false, true);
	setStyle(STY_COMMENTLINE,COLREF(0x00, 0x80, 0x00), false, true);
	setStyle(STY_STRING,     COLREF(0xA3, 0x15, 0x15), false, false);
	setStyle(STY_CHAR,       COLREF(0xA3, 0x15, 0x15), false, false);
	setStyle(STY_NUMBER,     COLREF(0x09, 0x86, 0x58), false, false);
	setStyle(STY_PREPROC,    COLREF(0x80, 0x00, 0x80), false, false);
	setStyle(STY_OPERATOR,   COLREF(0x00, 0x00, 0x00), false, false);

	codeEditor->setMarginTypeN(0, 1);
	updateLineNumberWidth();
	codeEditor->setMarginTypeN(1, 0);
	codeEditor->setMarginWidthN(1, 16);

	codeEditor->autoCSetSeparator('?');
	codeEditor->autoCSetIgnoreCase(true);
	codeEditor->autoCSetAutoHide(true);

	connect(codeEditor, &ScintillaEditBase::linesAdded, this, [this](Scintilla::Position) {
		updateLineNumberWidth();
	});
	connect(codeEditor, &ScintillaEditBase::charAdded, this, &Editor::onCharAdded);
	connect(codeEditor, &ScintillaEditBase::updateUi, this, &Editor::updateBrackets);
	connect(codeEditor, &ScintillaEditBase::styleNeeded, this, [this](Scintilla::Position position) {
		restyle(static_cast<long>(position));
	});

	ui->verticalLayout_9->addWidget(codeEditor);
	qDebug() << "[CODE] editor created & added";

	QString cppCode;
	if (canEditObject(robj)) {
		std::stringstream buffer;
		robj->SavePrimitive(buffer, "");
		cppCode = QString::fromStdString(buffer.str());
	}
	if (!cppCode.isEmpty()) {
		codeEditor->setText(cppCode.toUtf8().constData());
	} else {
		QString info = QString("// Object: %1 (%2)\n// Name: %3")
			.arg(robj->ClassName())
			.arg(robj->IsA()->GetName())
			.arg(QString::fromUtf8(robj->GetName()));
		codeEditor->setText(info.toUtf8().constData());
	}

	codeEditor->colourise(0, -1);
	qDebug() << "[CODE] setupCodeEditor end";
}

void Editor::restyle(long upTo) {
	qDebug() << "[CODE] restyle" << upTo;
	const long length = codeEditor->textLength();
	if (upTo > length)
		upTo = length;
	if (upTo <= 0)
		return;

	const QByteArray text = codeEditor->get_text_range(0, upTo);
	const QByteArray styles = styleCpp(text);
	const int n = styles.size();

	codeEditor->startStyling(0, 0);
	int i = 0;
	while (i < n) {
		const char st = styles.at(i);
		int j = i + 1;
		while (j < n && styles.at(j) == st) j++;
		codeEditor->setStyling(j - i, static_cast<unsigned char>(st));
		i = j;
	}
}

void Editor::onCharAdded(int ch) {
	qDebug() << "[CODE] onCharAdded" << ch;
	const char c = static_cast<char>(ch);
	if (c == '\r' || c == '\n') {
		autoIndentBraceBody(codeEditor, inEditAction);
		return;
	}

	inEditAction = true;
	autoClosePairStatic(codeEditor, c);
	inEditAction = false;

	if (std::isalpha(static_cast<unsigned char>(c)) || c == '_')
		maybeShowCompletion(codeEditor);
}

void Editor::updateBrackets() {
	qDebug() << "[CODE] updateBrackets";
	if (inEditAction)
		return;
	const long caret = codeEditor->currentPos();
	long bracePos = -1;
	if (char c = static_cast<char>(codeEditor->charAt(caret)); isBracketChar(c))
		bracePos = caret;
	else if (caret > 0) {
		const char c = static_cast<char>(codeEditor->charAt(caret - 1));
		if (isBracketChar(c))
			bracePos = caret - 1;
	}
	if (bracePos >= 0) {
		const long match = codeEditor->braceMatch(bracePos, 0);
		if (match >= 0)
			codeEditor->braceHighlight(bracePos, match);
		else
			codeEditor->braceBadLight(bracePos);
	} else {
		codeEditor->braceHighlight(-1, -1);
	}
}

void Editor::updateLineNumberWidth() {
	const long lines = codeEditor->lineCount();
	int digits = 1;
	long n = lines - 1;
	while (n >= 10) {
		digits++;
		n /= 10;
	}
	const long width = qBound(24L, 8L * digits + 8L, 70L);
	codeEditor->setMarginWidthN(0, width);
}

void Editor::on_lockcode_button_clicked() {
	if (!codeEditor)
		return;
	const bool locked = codeEditor->readOnly();
	codeEditor->setReadOnly(!locked);
}

