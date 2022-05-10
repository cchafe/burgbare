// chuck -s --srate:48000 resi.ck
SndBuf input;
SndBuf resi;

input.read("/tmp/testorig.wav");
input => blackhole;
// 48000::samp => now;

//4*128::samp => now;

resi.read("/tmp/testresi.wav");
resi => Gain g => Gain s;
g.gain(-1.3);

input => 
s => WvOut w => blackhole;
w.wavFilename("/tmp/sum.wav");

while (input.pos()<input.samples()) 1::samp => now;
<<<input.pos()>>>;

