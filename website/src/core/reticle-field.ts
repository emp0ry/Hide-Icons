import { samplePulses, type PulseFrame } from './pulses.ts';
export const LOOP_SECONDS = 24;
export const ART_KINDS = ['icons','cursor','lens','bridge','console','compass','refresh'] as const;
export type ArtKind = typeof ART_KINDS[number];
export interface ReticleFrame { phase:number; intro:number; pointerX:number; pointerY:number; strength:number; waves:PulseFrame }
const TAU=Math.PI*2;
const waveSample=new Float32Array(3);
const hash=(a:number,b:number)=>{const n=Math.sin(a*127.1+b*311.7)*43758.5453;return n-Math.floor(n);};
function polygon(x:number,y:number,vertices:number[][]){let inside=false;for(let i=0,j=vertices.length-1;i<vertices.length;j=i++){const [a,b]=vertices[i],[c,d]=vertices[j];if((b>y)!==(d>y)&&x<(c-a)*(y-b)/(d-b)+a)inside=!inside;}return inside;}
function line(x:number,y:number,a:number,b:number,c:number,d:number){const dx=c-a,dy=d-b,t=Math.max(0,Math.min(1,((x-a)*dx+(y-b)*dy)/(dx*dx+dy*dy)));return Math.hypot(x-a-t*dx,y-b-t*dy);}
function shape(kind:ArtKind,x:number,y:number):number|null{
 if(kind==='icons'){
  for(let row=0;row<3;row++)for(let col=0;col<3;col++){
   const cx=(col-1)*.62,cy=(row-1)*.62,ax=Math.abs(x-cx),ay=Math.abs(y-cy);
   if(Math.max(ax,ay)<.235&&Math.hypot(Math.max(0,ax-.16),Math.max(0,ay-.16))<.075)return (col-row)*.12;
  }
 }else if(kind==='cursor'){
  if(polygon(x,y,[[-.5,.9],[-.5,-.66],[-.12,-.3],[.16,-.91],[.39,-.81],[.13,-.19],[.68,-.15]]))return .10+x*.15;
  for(let j=0;j<3;j++){const d=Math.abs(y-(-.55+.18*j+.15*Math.sin(x*3+j*.4)));if(x<-.58&&x>-.96&&d<.028)return -.15-j*.08;}
 }else if(kind==='lens'){
  const a=Math.abs(x),edge=.72-.22*a;
  if(a<.9&&a>.035&&y<edge&&y>-edge)return a*.36-.22;
  if(line(x,y,-.82,.82,-.06,.98)<.018||line(x,y,.06,.98,.82,.82)<.018)return -.18;
 }else if(kind==='bridge'){
  for(const side of [-1,1]){
   if(Math.abs(x-side*.69)<.25&&Math.abs(y)<.46)return side*.2;
   if(Math.abs(x-side*.69)<.28&&Math.abs(y+.58)<.018)return -.1;
  }
  if(Math.abs(x)<.57){const route=x*.48;if(Math.abs(y-route)<.045||Math.abs(y+route)<.045)return .08;}
 }else if(kind==='console'){
  if(line(x,y,-.86,.67,-.16,0)<.09||line(x,y,-.16,0,-.86,-.67)<.09)return x*.16;
  if(x>.10&&x<.9&&Math.abs(y+.54)<.09)return .12;
  if(x>.10&&x<.83&&Math.abs(y-.55)<.018)return -.25;
 }else if(kind==='compass'){
  const a=Math.abs(x),b=Math.abs(y);
  if((a<.16&&b<.52)||(b<.16&&a<.52))return .18;
  if(polygon(a,b,[[.34,.08],[.75,.42],[.91,.26],[.91,.84],[.33,.84],[.49,.68],[.08,.34]]))return -.10;
 }else{
  for(let j=0;j<3;j++){
   const v=y+.48-j*.48;
   if(Math.abs(x)<.9&&Math.abs(v-(.28-Math.abs(x)*.55))<.07)return j*.22-.22;
  }
  if(Math.abs(x)<.70&&Math.abs(y+.9)<.017)return -.32;
 }
 return null;
}
export function createReticlePoints(detail=100,kind:ArtKind='icons'){
 const data:number[]=[],step=2/detail;
 for(let iy=0;iy<=detail;iy++)for(let ix=0;ix<=detail;ix++){
  const x=-1+ix*step+(hash(ix,iy)-.5)*step*.42,y=-1+iy*step+(hash(iy,ix+37)-.5)*step*.42;
  const z=shape(kind,x,y);if(z!==null)data.push(x,y,z,hash(ix+82,iy+51));
 }
 return new Float32Array(data);
}
export function sampleReticle(points:Float32Array,index:number,state:ReticleFrame,out:Float32Array){
 const ox=points[index],oy=points[index+1],oz=points[index+2],seed=points[index+3];
 const t=state.phase*TAU,ay=Math.sin(t)*.22,ax=Math.cos(t)*.10;
 const z=oz+Math.sin(t+ox*2.4+oy)*.045;
 let x=ox*Math.cos(ay)+z*Math.sin(ay),depth=z*Math.cos(ay)-ox*Math.sin(ay),y=oy*Math.cos(ax)-depth*Math.sin(ax);
 depth=oy*Math.sin(ax)+depth*Math.cos(ax);
 const dx=x-state.pointerX,dy=y-state.pointerY,d=Math.hypot(dx,dy),pull=Math.exp(-d*d*5)*state.strength;
 x+=dx*pull*.13;y+=dy*pull*.13;depth+=pull*.07;
 samplePulses(state.waves,x,y,1,1.85,.15,.08,.12,waveSample);
 x+=waveSample[0];y+=waveSample[1];const wave=waveSample[2];
 const intro=1-Math.pow(1-Math.min(1,state.intro),3),perspective=1/(1.75-depth*.30);
 out[0]=(x+(1-intro)*Math.sin(seed*TAU)*.18)*perspective*1.65;
 out[1]=(y+(1-intro)*Math.cos(seed*TAU)*.18)*perspective*1.65;
 out[2]=Math.min(.92,(.36+seed*.16+Math.sin(t+ox*1.7+oy)*.13+depth*.18+wave*.26)*intro);
 out[3]=(1.12+seed*.28)*perspective*1.65;
 return out;
}
