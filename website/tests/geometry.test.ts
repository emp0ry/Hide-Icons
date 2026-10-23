import test from'node:test';
import assert from'node:assert/strict';
import{ART_KINDS,createReticlePoints,sampleReticle,type ReticleFrame}from'../src/core/reticle-field.ts';
import{createPulseTrain}from'../src/core/pulses.ts';
for(const kind of ART_KINDS)test(`${kind}: deterministic silhouette closes its loop without clipping or invalid values`,()=>{
 const points=createReticlePoints(42,kind),a=new Float32Array(4),b=new Float32Array(4),waves=createPulseTrain(1.7);
 assert.deepEqual(points,createReticlePoints(42,kind));assert(points.length>300);
 const frame:ReticleFrame={phase:0,intro:1,pointerX:0,pointerY:0,strength:0,waves:waves.frame};
 for(let i=0;i<points.length;i+=4){sampleReticle(points,i,frame,a);frame.phase=1;sampleReticle(points,i,frame,b);frame.phase=0;for(let n=0;n<4;n++)assert(Math.abs(a[n]-b[n])<1e-5);assert(a.every(Number.isFinite));assert(Math.abs(a[0])<1.2&&Math.abs(a[1])<1.2);}
 waves.push(0,0);waves.advance(.2);waves.push(.2,.1);frame.strength=1;frame.pointerX=.2;frame.pointerY=.3;
 for(let phase=0;phase<1;phase+=.17){frame.phase=phase;for(let i=0;i<points.length;i+=4){sampleReticle(points,i,frame,a);assert(a.every(Number.isFinite));assert(a[2]>=0&&a[2]<=1);assert(Math.abs(a[0])<1.5&&Math.abs(a[1])<1.5);}}
});
test('rapid input preserves existing wave ages and remains bounded',()=>{const w=createPulseTrain(1.7);w.push(0,0);w.advance(.2);const age=w.frame.data[2];for(let i=0;i<100;i++)w.push(.1,.2);assert.equal(w.frame.count,32);assert.equal(w.frame.data[2],age);w.advance(2);assert.equal(w.frame.count,0);});
