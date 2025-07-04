# WLEditor Key Map Reference

Complete reference for WordStar-like key bindings in WLEditor.

## Philosophy

WordStar's key bindings were designed for efficient text editing without moving hands from the home row. The diamond pattern (E/S/D/X) provides cursor movement, while Ctrl+Q and Ctrl+K sequences give access to extended functions.

## Basic Navigation (Diamond Pattern)

| Key | Action | Description |
|-----|--------|-------------|
| **Ctrl+E** | Move up | Move cursor up one line |
| **Ctrl+S** | Move left | Move cursor left one character |
| **Ctrl+D** | Move right | Move cursor right one character |
| **Ctrl+X** | Move down | Move cursor down one line |

### Why Diamond Pattern?
E (Up)
↑
S ← ■ → D
↓
X (Down)

Keeps fingers on home row for maximum typing efficiency.

## Extended Navigation (Ctrl+Q Sequences)

| Key Sequence | Action | Description |
|--------------|--------|-------------|
| **Ctrl+Q, R** | Beginning of file | Move to start of document |
| **Ctrl+Q, C** | End of file | Move to end of document |
| **Ctrl+Q, S** | Beginning of line | Move to start of current line |
| **Ctrl+Q, D** | End of line | Move to end of current line |
| **Ctrl+Q, E** | Top of screen | Move to top of visible area |
| **Ctrl+Q, X** | Bottom of screen | Move to bottom of visible area |

## Text Selection and Editing (Ctrl+K Sequences)

### Block Operations

| Key Sequence | Action | Description |
|--------------|--------|-------------|
| **Ctrl+K, B** | Mark block begin | Set start of selection |
| **Ctrl+K, K** | Mark block end / Copy | Set end of selection or copy to clipboard |
| **Ctrl+K, Y** | Delete block / Delete line | Delete selection or current line |
| **Ctrl+K, C** | Copy block | Copy selection to clipboard |

### Clipboard Operations

| Key Sequence | Action | Description |
|--------------|--------|-------------|
| **Ctrl+K, C** | Paste | Paste from clipboard |
| **Ctrl+K, V** | Paste with history | Show clipboard history for selection |

## Multi-Level Clipboard

WLEditor extends WordStar with a 10-item clipboard history:

1. **Ctrl+K, V** - Opens clipboard history dialog
2. **Number keys (0-9)** - Select clipboard item
3. **Enter** - Paste selected item
4. **Escape** - Cancel

### Clipboard Behavior

- Every copy/cut operation adds to history
- History persists during session
- Most recent item is always #0
- Duplicate entries are merged

## Modern Additions

| Key | Action | Description |
|-----|--------|-------------|
| **Ctrl+Z** | Undo | Undo last action |
| **Ctrl+Y** | Redo | Redo undone action |
| **Ctrl+F** | Find | Open find dialog |
| **Ctrl+H** | Replace | Open find and replace dialog |
| **Ctrl+O** | Open file | Open file dialog |
| **Ctrl+S** | Save file | Save current document |
| **Ctrl+N** | New file | Create new document |

## Special Functions

### Word Operations

| Key Sequence | Action | Description |
|--------------|--------|-------------|
| **Ctrl+Q, F** | Find | Search for text |
| **Ctrl+Q, A** | Find and replace | Search and replace text |

### Line Operations

| Key | Action | Description |
|-----|--------|-------------|
| **Ctrl+Y** | Delete line | Delete current line |
| **Enter** | New line | Insert line break |
| **Tab** | Insert tab | Insert tab character or spaces |

## Configuration Notes

### Wrap Mode

WLEditor uses character-based wrapping:
- Default: 80 characters
- Configurable via preferences
- Wraps at character boundary, not word boundary (WordStar style)

### Font Handling

- Default: Noto Sans Mono CJK JP 12pt
- Supports full Unicode including CJK characters
- Monospace font recommended for proper alignment

## Tips for Efficiency

### 1. Stay on Home Row
Keep hands positioned on ASDF and JKL; keys. All WordStar commands are accessible without hand movement.

### 2. Learn Sequences
Practice Ctrl+Q and Ctrl+K sequences until they become muscle memory.

### 3. Use Clipboard History
Take advantage of the 10-item clipboard for complex editing tasks.

### 4. Block Selection
Use Ctrl+K,B and Ctrl+K,K for precise text selection without mouse.

## Differences from Original WordStar

### Enhanced Features

- **Multi-level clipboard** (original had single clipboard)
- **Unlimited undo/redo** (original was limited)
- **Unicode support** (original was ASCII only)
- **Modern file dialogs** (original used command-line style)

### Preserved Features

- **Diamond navigation** (exact same keys)
- **Two-level commands** (Ctrl+Q, Ctrl+K sequences)
- **Character-based wrapping** (not word-based)
- **Block operations** (same workflow)

## Quick Reference Card
NAVIGATION          EXTENDED NAV        BLOCK OPS
Ctrl+E = Up         Ctrl+Q,R = File top  Ctrl+K,B = Mark start
Ctrl+S = Left       Ctrl+Q,C = File end  Ctrl+K,K = Mark end
Ctrl+D = Right      Ctrl+Q,S = Line start Ctrl+K,Y = Delete
Ctrl+X = Down       Ctrl+Q,D = Line end   Ctrl+K,C = Paste
Ctrl+K,V = Paste menu
MODERN KEYS
Ctrl+Z = Undo       Ctrl+F = Find       Ctrl+O = Open
Ctrl+Y = Redo       Ctrl+H = Replace    Ctrl+S = Save

## Learning Path

1. **Start with diamond navigation** (E/S/D/X)
2. **Add line navigation** (Ctrl+Q,S and Ctrl+Q,D)
3. **Learn block selection** (Ctrl+K,B and Ctrl+K,K)
4. **Master clipboard** (Ctrl+K,C and Ctrl+K,V)
5. **Use extended navigation** (Ctrl+Q sequences)

This progression builds muscle memory gradually while maintaining productivity.
