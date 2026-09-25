"""Original layered SFX and a 32-second seamless instrumental loop. No external recordings."""
from pathlib import Path
import numpy as np
import wave
RATE=44100
OUT=Path(__file__).parent/'Audio'
OUT.mkdir(exist_ok=True)
rng=np.random.default_rng(813)
def save(name,y):
    y=np.asarray(y)
    if y.ndim==1:y=np.column_stack([y,y])
    y=np.tanh(y*1.1)*.8
    with wave.open(str(OUT/(name+'.wav')),'wb') as f:
        f.setnchannels(2);f.setsampwidth(2);f.setframerate(RATE);f.writeframes((y*32767).astype('<i2').tobytes())
def time(d):return np.arange(int(RATE*d))/RATE
def bell(freq,d=.45):
    t=time(d);return (np.sin(2*np.pi*freq*t)*np.exp(-t*9)+.3*np.sin(2*np.pi*freq*2.76*t)*np.exp(-t*22))*(1-np.exp(-t*300))
def sequence(notes,step,d):
    y=np.zeros(int(RATE*d))
    for i,f in enumerate(notes):
        p=int(i*step*RATE);v=bell(f,min(.7,d-i*step));v=v[:len(y)-p];y[p:p+len(v)]+=v*.4
    return y
save('Select',bell(880,.13)*.3)
t=time(.20);save('Swap',np.sin(2*np.pi*(390*t+800*t*t))*np.sin(np.pi*t/.20)**2*.3)
save('Pop',sequence([659,988,1318],.045,.32))
save('Combo',sequence([523,659,784,1047],.07,.7))
save('Rescue',sequence([784,1047,1318,1568],.09,.85))
save('Win',sequence([523,659,784,1047,784,1047,1318,1568],.16,2.2))
save('Lose',sequence([523,440,392,330],.18,1.1)*.55)
t=time(.3);noise=rng.normal(0,1,len(t));noise=np.convolve(noise,np.ones(12)/12,'same')
save('Mud',(noise*.5+np.sin(2*np.pi*(180*t-210*t*t))*.25)*np.sin(np.pi*t/.3)**2)
t=time(.7);save('Burst',(rng.normal(0,.1,len(t))+np.sin(2*np.pi*(160*t+190*t*t))*.25)*np.exp(-t*7)+sequence([1047,1318,1568],.06,.7)*.5)
t=time(.42);phase=2*np.pi*(360*t-125*t*t)
env=np.exp(-((t-.085)/.04)**2)+.72*np.exp(-((t-.24)/.05)**2)
bark=(np.sin(phase)+.55*np.sin(phase*2)+.3*np.sin(phase*3)+rng.normal(0,.14,len(t)))*env*.35
save('Bark',bark)
# 120 BPM, sixteen bars. A restrained mallet melody over warm plucked chords.
duration=32;music=np.zeros((int(duration*RATE),2))
def add(v,start,gain,pan=0):
    p=int(start*RATE);v=v[:len(music)-p]*gain
    music[p:p+len(v),0]+=v*(1-pan*.4);music[p:p+len(v),1]+=v*(1+pan*.4)
chords=[[261.63,329.63,392],[220,261.63,329.63],[174.61,220,261.63],[196,246.94,293.66]]
melodies=[[523,659,784,659,587,659,523,0],[659,784,880,784,659,523,440,0],[523,698,880,698,659,523,587,0],[587,784,988,784,659,587,523,0]]
for bar in range(16):
    c=chords[(bar//2)%4];m=melodies[(bar//2)%4]
    for beat in range(4):
        at=bar*2+beat*.5
        for k,f in enumerate(c):add(bell(f,.65),at+k*.025,.065,(-1)**k*.6)
        tb=time(.35);add(np.sin(2*np.pi*c[0]/2*tb)*np.exp(-tb*12),at,.14)
        th=time(.055);add(rng.normal(0,1,len(th))*np.exp(-th*80),at+.25,.018,.5)
    for n,f in enumerate(m):
        if f:add(bell(f,.38),bar*2+n*.25,.105,(-1)**n*.45)
# Fade only the final tail into the beginning to keep the loop seam quiet.
fade=int(.04*RATE);music[:fade]*=np.linspace(0,1,fade)[:,None];music[-fade:]*=np.linspace(1,0,fade)[:,None]
save('Music',music)
t=time(16);ambient=rng.normal(0,.007,len(t));ambient=np.convolve(ambient,np.ones(35)/35,'same')
for at in [1.2,4.7,8.1,11.8,14.1]:
    tc=time(.35);v=np.sin(2*np.pi*(1800*tc+900*tc*tc))*np.sin(np.pi*tc/.35)**2*.018;p=int(at*RATE);ambient[p:p+len(v)]+=v
ambient[:fade]*=np.linspace(0,1,fade);ambient[-fade:]*=np.linspace(1,0,fade)
save('Ambience',ambient)
print('Generated',len(list(OUT.glob('*.wav'))),'original stereo sound assets')

