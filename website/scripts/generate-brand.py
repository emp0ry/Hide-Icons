"""Generate original point artwork and outlined social metadata. fonttools[woff] + ImageMagick."""
from pathlib import Path
from fontTools.ttLib import TTFont
from fontTools.pens.svgPathPen import SVGPathPen
from fontTools.varLib.instancer import instantiateVariableFont
import json,subprocess

site=json.loads(Path('site.config.json').read_text())
source=TTFont('src/public/site/fonts/Geist.woff2')
cmap=source.getBestCmap();upm=source['head'].unitsPerEm
fonts={w:instantiateVariableFont(source,{'wght':w},inplace=False).getGlyphSet()for w in[500,600,900]}
def text(value,x,y,size,weight=500,fill='#f4f2f7',tracking=-.025,max_width=None):
 glyphs=fonts[weight];advance=0;paths=[]
 for c in value:
  name=cmap[ord(c)];pen=SVGPathPen(glyphs);glyphs[name].draw(pen)
  paths.append(f'<path transform="translate({advance:.2f})" d="{pen.getCommands()}"/>');advance+=glyphs[name].width+tracking*upm
 if max_width:size=min(size,max_width*upm/advance)
 return f'<g fill="{fill}" transform="translate({x} {y}) scale({size/upm} {-size/upm})">'+''.join(paths)+'</g>'
code=f'''import{{createReticlePoints,sampleReticle}}from'./src/core/reticle-field.ts';
const points=createReticlePoints(56,{json.dumps(site['kind'])}),out=new Float32Array(4),data=[];
const frame={{phase:0,intro:1,pointerX:0,pointerY:0,strength:0,waves:{{data:new Float32Array(96),count:0}}}};
for(let i=0;i<points.length;i+=4){{sampleReticle(points,i,frame,out);data.push(Array.from(out));}}
process.stdout.write(JSON.stringify(data));'''
points=json.loads(subprocess.check_output(['node','--input-type=module','-e',code],text=True))
def field(x,y,scale,radius):return '<g fill="#b7a0e5">'+''.join(f'<circle cx="{x+p[0]*scale:.2f}" cy="{y-p[1]*scale:.2f}" r="{p[3]*radius:.2f}" opacity="{p[2]:.2f}"/>'for p in points)+'</g>'
fallback=('<svg xmlns="http://www.w3.org/2000/svg" width="1000" height="360" viewBox="0 0 1000 360">'+field(500,180,300,.9))if site['kind']=='bridge'else('<svg xmlns="http://www.w3.org/2000/svg" width="600" height="600" viewBox="0 0 600 600">'+field(300,300,240,.8))
Path('src/public/site/field.svg').write_text(fallback+'</svg>')
parts=['<svg xmlns="http://www.w3.org/2000/svg" width="1200" height="630" viewBox="0 0 1200 630"><rect width="1200" height="630" fill="#09090d"/>',text('emp0ry.',64,70,28,900,tracking=-.07),text('/ INDEPENDENT SOFTWARE',204,68,11,fill='#aaa6b5',tracking=.08),field(932,285,205,.85),text(site['name'],64,243,86,600,tracking=-.055,max_width=650)]
for i,line in enumerate(site['headline']):parts.append(text(line,68,327+i*50,34,fill='#b7a0e5'if i else'#f4f2f7',max_width=650))
parts+=['<path d="M64 508H1136" stroke="#c7bcda" stroke-opacity=".24"/>',text('Open source / By emp0ry',68,554,15,fill='#aaa6b5'),text(site['host'],770,554,15,fill='#cbb6ef',max_width=365),'</svg>']
Path('design/social-preview.svg').write_text('\n'.join(parts))
subprocess.run(['magick','-background','#09090d','design/social-preview.svg','-depth','8','src/public/site/social-preview.png'],check=True)
icons={'icons':'M6 6h8v8H6zM18 6h8v8h-8zM6 18h8v8H6zM18 18h8v8h-8z','cursor':'M9 4v23l6-6 5 9 4-2-5-9h9L9 4Z','lens':'M16 8 3 5v20l13 3 13-3V5L16 8Zm0 0v20','bridge':'M2 9h8v14H2zM22 9h8v14h-8zM10 12l12 8M10 20l12-8','console':'m5 6 10 10L5 26m14-2h10','compass':'M16 3v26M3 16h26M12 7l4-4 4 4M25 12l4 4-4 4M12 25l4 4 4-4M7 12l-4 4 4 4','refresh':'m3 7 13-4 13 4-13 4L3 7Zm0 9 13 4 13-4M3 25l13 4 13-4'}
Path('src/public/site/icon.svg').write_text(f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 40 40"><rect width="40" height="40" rx="9" fill="#111116"/><path d="{icons[site["kind"]]}" transform="translate(4 4)" stroke="#cbb6ef" fill="none" stroke-width="1.4" stroke-linejoin="round" stroke-linecap="round"/></svg>')
print(site['name']+': original SVG artwork, icon and 1200×630 social PNG generated.')
