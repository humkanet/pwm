XC="C:/Program Files (x86)/Microchip/xc8/v1.45/bin"
CC=$(XC)/xc8.exe

NAME     = pwm
MCU      = 16f1765

FOSC     = 32000000UL
LFINTOSC = 31000UL
HFINTOSC = 16000000UL

OBJ_DIR  = ./obj

CFLAGS=--chip=$(MCU) -D_XTAL_FREQ=$(FOSC) -D_LFINTOSC_FREQ=$(LFINTOSC) -D_HFINTOSC_FREQ=$(HFINTOSC) \
	-Q --mode=pro --debugger=none --opt=asm,space --dep=none --objdir=$(OBJ_DIR)

all: $(OBJ_DIR) $(NAME).cof

$(NAME).cof: main.p1 pwm.p1 pcf8574.p1 i2c.p1 lcd.p1 str.p1 options.p1 ui.p1 btn1.p1 btn2.p1 \
		btn3.p1 btn4.p1 btn5.p1 enc1.p1 enc2.p1
	$(CC) $(CFLAGS) --output=mcof,inhx32 -O$@ $^

%.p1: %.c %.h
	$(CC) $(CFLAGS) --pass1 -C -O$@ $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(NAME).hex $(NAME).cof *.p1 *.pre *.as *.cmf *.hxl *.obj *.sdb *.sym
