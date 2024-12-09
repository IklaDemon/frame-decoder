cc = clang

frame-decoder.exe: main.c decoderLib.o
	$(cc) main.c -o frame-decoder.exe decoderLib.o

decoderLib.o: decoderLib.c decoderLib.h
	$(cc) -c decoderLib.c -o decoderLib.o

clean:
	@del *.o
	@del frame-decoder.exe
	@echo deleted all bin files