MyProductName = "SSHFS-Win"
MyCompanyName = "Navimatics LLC"
MyDescription = "SSHFS for Windows"
MyProductVersion = "2021.1 Beta2"
MyProductStage = "Beta"
MyVersion = 3.7.$(shell date '+%y%j')
ifeq ($(shell uname -m),x86_64)
	MyArch = x64
else
	MyArch = x86
endif

CertIssuer = "DigiCert"
CrossCert = "DigiCert High Assurance EV Root CA.crt"

PrjDir	= $(shell pwd)
BldDir	= .build/$(MyArch)
DistDir = $(BldDir)/dist
SrcDir	= $(BldDir)/src
RootDir	= $(BldDir)/root
WixDir	= $(BldDir)/wix
Status	= $(BldDir)/status
BinExtra= ssh #bash ls mount

export PATH := $(shell cygpath -au "$$WIX")/bin:$(PATH)

goal: $(Status) $(Status)/done

$(Status):
	mkdir -p $(Status)

$(Status)/done: $(Status)/dist
	touch $(Status)/done

$(Status)/dist: $(Status)/wix
	mkdir -p $(DistDir)
	cp $(shell cygpath -aw $(WixDir)/sshfs-win-$(MyVersion)-$(MyArch).msi) $(DistDir)
	tools/signtool sign \
		/ac tools/$(CrossCert) \
		/i $(CertIssuer) \
		/n $(MyCompanyName) \
		/d $(MyDescription) \
		/fd sha1 \
		/t http://timestamp.digicert.com \
		'$(shell cygpath -aw $(DistDir)/sshfs-win-$(MyVersion)-$(MyArch).msi)' || \
		echo "SIGNING FAILED! The product has been successfully built, but not signed." 1>&2
	touch $(Status)/dist

$(Status)/wix: $(Status)/sshfs-win sshfs-win.wxs
	mkdir -p $(WixDir)
	cp sshfs-win.wxs $(WixDir)/
	candle -nologo -arch $(MyArch) -pedantic\
		-dMyProductName=$(MyProductName)\
		-dMyCompanyName=$(MyCompanyName)\
		-dMyDescription=$(MyDescription)\
		-dMyProductVersion=$(MyProductVersion)\
		-dMyProductStage=$(MyProductStage)\
		-dMyVersion=$(MyVersion)\
		-dMyArch=$(MyArch)\
		-o "$(shell cygpath -aw $(WixDir)/sshfs-win.wixobj)"\
		"$(shell cygpath -aw $(WixDir)/sshfs-win.wxs)"
	heat dir $(shell cygpath -aw $(RootDir))\
		-nologo -dr INSTALLDIR -cg C.Main -srd -ke -sreg -gg -sfrag\
		-o $(shell cygpath -aw $(WixDir)/root.wxs)
	candle -nologo -arch $(MyArch) -pedantic\
		-dMyProductName=$(MyProductName)\
		-dMyCompanyName=$(MyCompanyName)\
		-dMyDescription=$(MyDescription)\
		-dMyProductVersion=$(MyProductVersion)\
		-dMyProductStage=$(MyProductStage)\
		-dMyVersion=$(MyVersion)\
		-dMyArch=$(MyArch)\
		-o "$(shell cygpath -aw $(WixDir)/root.wixobj)"\
		"$(shell cygpath -aw $(WixDir)/root.wxs)"
	light -nologo\
		-o $(shell cygpath -aw $(WixDir)/sshfs-win-$(MyVersion)-$(MyArch).msi)\
		-ext WixUIExtension\
		-b $(RootDir)\
		$(shell cygpath -aw $(WixDir)/root.wixobj)\

$(Status)/sshfs-win: sshfs-win.c
	gcc -o sshfs-win.exe sshfs-win.c -lwinfsp -lcygwin

PLAN TO FOLLOW:
{
  "analysis": "The issue is that direct UNC access to SSHFS-Win does not work without mapping a drive first. This is likely due to the way SSHFS-Win is implemented, which relies on the `net use` command to establish a connection. To fix this, we need to modify the SSHFS-Win code to allow direct UNC access without mapping a drive.",
  "approach": "We will modify the SSHFS-Win code to handle direct UNC access by implementing a custom UNC handler that can establish a connection to the SSHFS host without relying on the `net use` command.",
  "changes": [
    {
      "file": "sshfs-win.c",
      "action": "modify",
      "summary": "Implement custom UNC handler to establish connection to SSHFS host",
      "dependencies": [
        "WinFsp API",
        "Cygwin POSIX environment"
      ],
      "risk": "medium"
    },
    {
      "file": "sshfs-win.c",
      "action": "modify",
      "summary": "Update SSHFS-Win code to handle direct UNC access",
      "dependencies": [
        "custom UNC handler"
      ],
      "risk": "medium"
    },
    {
      "file": "Makefile",
      "action": "modify",
      "summary": "Update Makefile to build custom UNC handler",
      "dependencies": [
        "sshfs-win.c"
      ],
      "risk": "low"
    },
    {
      "file": "README.md",
      "action": "modify",
      "summary": "Update README to reflect changes to SSHFS-Win",
      "dependencies": [
        "custom UNC handler"
      ],
      "risk": "low"
    }
  ],
  "test_strategy": "We will test the custom UNC handler by creating a test UNC path and verifying that it can be accessed without mapping a drive. We will also test the updated SSHFS-Win code to ensure that it handles direct UNC access correctly.",
  "estimated_complexity": "medium"
}