# sequencer_display.py -- picostepseq displayio object
# 9 Aug 2022 - @todbot / Tod Kurt
# Part of picostepseq : https://github.com/todbot/picostepseq/

import displayio
import terminalio
import vectorio

from adafruit_display_text import bitmap_label as label
from adafruit_bitmap_font import bitmap_font

uidebug = False

four_per_line = False  # normally we do all 8 steps in one line

if four_per_line:
    step_text_pos = ( (10,12), (40,12), (70,12), (100,12),
                      (10,35), (40,35), (70,35), (100,35) )
    bpm_text_pos = (0, 57)
    bpm_val_pos = (25, 57)
    trans_text_pos = (55, 57)
    trans_val_pos = (80, 57)
    seqno_text_pos = (0, 48)
    play_text_pos = (110, 57)
    oct_text_offset = (15,3)  # four per line
    gate_bar_offset = (0,-12)
    gate_bar_width, gate_bar_height = (14,4)
    edit_text_offset = (18,-3)
else:
    # eight all across
    step_text_pos = ( (0,12), (16,12), (32,12), (48,12),
                      (64,12), (80,12), (96,12), (112,12) )
    bpm_text_pos = (0, 57)
    bpm_val_pos = (25, 57)
    trans_text_pos = (55, 57)
    trans_val_pos = (80, 57)
    seqno_text_pos = (0, 45)
    play_text_pos = (110, 57)
    oct_text_offset = (3,12)
    gate_bar_offset = (0,-12)
    gate_bar_width, gate_bar_height = (14,4)
    edit_text_offset = (3,22)

class SequencerDisplay(displayio.Group):
    def __init__(self, sequencer):
        super().__init__(x=0,y=0,scale=1)
        self.seq = sequencer
        self.setup()

    def setup(self):
        gate_pal = displayio.Palette(1)
        gate_pal[0] = 0xffffff
        self.notegroup = displayio.Group()
        self.octgroup = displayio.Group()
        self.editgroup = displayio.Group()
        self.gategroup = displayio.Group()
        self.append(self.notegroup)
        self.append(self.octgroup)
        self.append(self.editgroup)
        self.append(self.gategroup)
        font = bitmap_font.load_font("helvB12.pcf")
        #font = terminalio.FONT
        font2 = terminalio.FONT
        for (x,y) in step_text_pos:
            self.notegroup.append( label.Label(font, text="A#", x=x, y=y, line_spacing=0.65))
            self.octgroup.append( label.Label(font2, text="0", x=x+oct_text_offset[0], y=y+oct_text_offset[1]))
            self.editgroup.append( label.Label(font2, text="*", x=x+edit_text_offset[0], y=y+edit_text_offset[1]))
            self.gategroup.append( vectorio.Rectangle(pixel_shader=gate_pal,
                                                      width=gate_bar_width, height=gate_bar_height,
                                                      x=x+gate_bar_offset[0], y=y+gate_bar_offset[1]))

        self.seqno_text = label.Label(font2, text="seqno", x=seqno_text_pos[0], y=seqno_text_pos[1])
        self.bpm_text = label.Label(font2, text="bpm:", x=bpm_text_pos[0], y=bpm_text_pos[1])
        self.bpm_val = label.Label(font2, text="bpm:", x=bpm_val_pos[0], y=bpm_val_pos[1])
        self.transpose_text = label.Label(font2, text="trans", x=trans_text_pos[0], y=trans_text_pos[1])
        self.transpose_val = label.Label(font2, text="+0", x=trans_val_pos[0], y=trans_val_pos[1])
        self.play_text = label.Label(font, text="||", x=play_text_pos[0], y=play_text_pos[1])
        self.append(self.bpm_text)
        self.append(self.bpm_val)
        self.append(self.transpose_text)
        self.append(self.transpose_val)
        self.append(self.play_text)
        self.append(self.seqno_text)

    def update_ui_step(self, step=None, n=0, v=127, gate=8, on=True, selected=False):
        if step is None:  # get current value
            step = self.seq.i
            n,v,gate,on = self.seq.steps[step]
        if uidebug: print("udpate_disp_step:", step,n,v,gate,on )
        (notename,octave) = self.seq.notenum_to_noteoct(n)
        notestr = notename
        octstr = str(octave)
        editstr = "^" if selected else '*' if not on else ' '
        if notestr != self.notegroup[step].text:
            self.notegroup[step].text = notestr
        if octstr != self.octgroup[step].text:
            self.octgroup[step].text = octstr
        self.editgroup[step].text = editstr
        self.gategroup[step].width = 1 + gate * gate_bar_width // 16

    def update_ui_steps(self):
        for i in range(self.seq.step_count):
            (n,v,gate,on) = self.seq.steps[i]
            self.update_ui_step(i, n, v, gate, on)

    def update_ui_bpm(self):
        self.bpm_val.text = "%d" % self.seq.tempo  # just update the part that changes

    def update_ui_playing(self):
        self.play_text.text = " >" if self.seq.playing else "||"

    def update_ui_transpose(self):
        try:
            self.transpose_val.text = "%+2d" % self.seq.transpose
        except AttributeError:
            pass
        
        
    def update_ui_seqno(self, msg=None):
        self.seqno_text.text = msg or f"seq: {self.seq.seqno+1}"  # 1-index for humans, matches silkscreen

    def update_ui_all(self):
        self.update_ui_seqno()
        self.bpm_text.text = "bpm:"
        self.transpose_text.text = "trs:"
        self.update_ui_bpm()
        self.update_ui_playing()
        self.update_ui_transpose()
        self.update_ui_steps()
