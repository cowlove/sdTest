#include "jimlib.h"
#include "serialLog.h"
#ifndef CSIM
#include "mySD.h"
#endif

JStuff j;
CLI_VARIABLE_FLOAT(x, 1);

void setup() {
    j.begin();
    OUT("calling SD.begin()");
    int r = SD.begin(4,2,1,3);
    OUT("SD begin() returned %d", r);
    j.cli.on("RESET", [](){ ESP.restart(); });
}

int writeS(ext::File f, string s) { return f.write(s.c_str()); }

int loopCount = 0;
void loop() {
    j.run();
    loopCount++;
    string fn = sfmt("/TEST%04d.TXT", loopCount % 10);
    ext::File f = SD.open(fn.c_str(), F_WRONLY | F_CREAT | F_TRUNC);
    int wres = writeS(f, sfmt("%08d", loopCount));
    f.close();

    f = SD.open(fn.c_str(), FILE_READ);
    uint8_t buf[64];
    int n = f.read(buf, sizeof(buf));
    f.close();
    if (n >= 0) {
        buf[n] = 0;
        OUT("write of '%s' returned %d, read returned '%s'", fn.c_str(), wres, buf);
    } else { 
        OUT("write of '%s' returned %d, read error %d", fn.c_str(), wres, n);
    }
    delay(1000);
}

#ifdef CSIM
class SketchCsim : public Csim_Module {
    public:
    void setup() { HTTPClient::csim_onPOST("http://.*/log", 
        [](const char *url, const char *hdr, const char *data, string &result) {
 	return 200; }); }
    string dummy;
    void parseArg(char **&a, char **la) override { if (strcmp(*a, "--dummy") == 0) dummy = *(++a); }
    void loop() override {}
} sketchCsim;
#endif
 
