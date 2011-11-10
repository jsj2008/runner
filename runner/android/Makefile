all: fix debug

debug: ndk
	@ant debug install

release: ndk
	@ant release install

ndk:
	@ndk-build

install:
	@ant installd

clean:
	@ndk-build clean
	@ant clean

fix:
	@find . -name "*.java" -print0 | xargs -0 astyle --style=java --indent=spaces=3 --brackets=linux --break-closing-brackets --suffix=none --formatted
	@find . -name "*.c" -print0 | xargs -0 astyle --indent=spaces=3 --brackets=break --break-closing-brackets --suffix=none --formatted

