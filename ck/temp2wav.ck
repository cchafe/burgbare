// chuck -s dataReaderColumn.ck temp2wav.ck

/*
cd ck
chuck -s dataReaderColumn.ck temp2wav.ck
cp ../TIKSI.dat.wav /tmp/t.wav 

in ../
time ./burgBare t 0 32 0 6 52 100 0 1 1
time ./burgBare t 0 32 3 6 22 100 0 1 1

compare immediate prediction in 
/tmp/testorig.snd
/tmp/testfade.snd
*/

//"../zagreb-monthly.dat" => string filename;
"../TIKSI.dat" => string filename;
<<<filename>>>;
1.0 => float downSamp;
if (true) // me.args()) 
{ 
  DataReader data;
  data.setDataSource(filename); // file name is assumed to be first argument
  data.start(1);
  data.scaledVal();
  Step step => Envelope env => WvOut w => blackhole;
  env.duration(downSamp::samp);
  w.wavFilename(filename+".wav");
  now => time start;
  start +0::samp => time sinDelay; // good sync from default
  SinOsc s;
  while (!data.isFinished())
  {
    if (now>sinDelay) {
      s => blackhole;
      s.gain(0.5);
      s.freq(48000.0/12.0);
    }

    data.scaledVal() => float val; // next data point, scaled in 0.0 - 1.0 range
    0.5 -=> val;
    2.0 *=> val;
    env.target(val - s.last());
    downSamp::samp => now;
  }
}

if (false) // me.args()) 
{ 
  DataReader data;
  data.setDataSource(filename); // file name is assumed to be first argument
  data.start(1);
  data.scaledVal();
  SinOsc s => dac;
  while (!data.isFinished())
  {
    data.scaledVal() => float val; // next data point, scaled in 0.0 - 1.0 range
    s.gain(val*0.1);
    s.freq(Std.mtof(80.0 + val*20.0));
    10::ms => now;
  }
}
