# func_80109E94 — append-to-tail of arg0->_10 sibling chain

Allocates via func_80008A30(arg0), splices the new node onto the
end of arg0->_10's `->_8` sibling chain (or sets it as the head if
empty), wires up _4/_8/_10/_14 (parent)/_50, then calls
func_80009D0C(new).

## Diff

The body has the right calls + field writes, but the chain-walk
reg routing differs: base routes `cur` through `a0` and the
"check head->_8" case through delay slots; built picks different
t-regs and the sub-case branch shape doesn't peel quite the same
way.

## Permuter hint

Try the goto-out form used in func_80108024 — `do { if (cur->_8 == 0)
goto found; cur = cur->_8; } while (1); found: ...` — to coax the
beql+delay-slot peeling pattern.
