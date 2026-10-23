import assert from'node:assert/strict';
import{readFileSync,existsSync}from'node:fs';
import{resolve}from'node:path';
import{createHash}from'node:crypto';
const read=f=>readFileSync(f,'utf8'),sha=x=>createHash('sha256').update(x).digest('hex');
const site=JSON.parse(read('site.config.json')),baseline=JSON.parse(read('design/baseline.json')),release=JSON.parse(read('verification/release-snapshot.json'));
assert.equal(release.repository,site.repository);for(const asset of site.assets)assert(release.assets.some(a=>a.name===asset&&a.state==='uploaded'&&a.size>0),'Verified release artifact '+asset);
const main=read('dist/index.html'),previous=read('design/previous-index.html');
const ids=[...main.matchAll(/\bid="([^"]+)"/g)].map(m=>m[1]);assert.equal(ids.length,new Set(ids).size,'Unique IDs');
for(const [,id]of previous.matchAll(/<section[^>]*\bid="([^"]+)"/g))assert(ids.includes(id),`Retain #${id}`);
assert(main.includes('mailto:support@emp0ry.com'),'Product support contact');
assert(!/<(?:form|input|video)\b/.test(main),'No credential forms, fake app controls or autoplay media');
assert(!/id="(?:gallery|install)-panel-[^"]+"[^>]*\bhidden/.test(main),'No-JS content remains available');
for(const file of Object.keys(baseline.retained)){if(['robots.txt','sitemap.xml'].includes(file))continue;assert.equal(sha(readFileSync(resolve('../docs',file))),baseline.retained[file],`Retain ${file}`);}
for(const name of['index','404']){
 const html=read(`dist/${name}.html`);assert.equal((html.match(/<h1\b/g)||[]).length,1,'One primary heading');
 for(const tag of['description','og:title','og:image','og:image:width','og:image:height','og:image:alt','twitter:card','theme-color'])assert(html.includes(`="${tag}"`),`${name}: ${tag}`);
 assert(html.includes(`https://${site.host}/`),'Correct domain');assert(!/fonts\.googleapis|fonts\.gstatic|googletagmanager/.test(html),'Self-hosted fonts; no invented analytics');
 const policy=html.match(/http-equiv="Content-Security-Policy" content="([^"]+)"/)[1];assert(!/unsafe-inline|unsafe-eval/.test(policy),'Restricted CSP');
 for(const [,script]of html.matchAll(/<script\b(?![^>]*\bsrc=)[^>]*>([\s\S]*?)<\/script>/g))assert(policy.includes(`'sha256-${createHash('sha256').update(script).digest('base64')}'`),'Exact CSP hashes');
 for(const [,asset]of html.matchAll(/(?:src|href)="(\/(?!\/)[^"?#]+)"/g))assert(existsSync(resolve('dist',asset.slice(1))),`${name}: asset ${asset}`);
 for(const [,id]of html.matchAll(/href="#([^"]+)"/g))assert(html.includes(`id="${id}"`),`${name}: anchor ${id}`);
 for(const [,attributes]of html.matchAll(/(<a\b[^>]*target="_blank"[^>]*>)/g))assert(attributes.includes('noopener'),'External tabs protected');
}
for(const [,asset]of main.matchAll(/data-asset="([^"]+)"/g))assert(site.assets.includes(asset),'Only verified published formats');
const png=readFileSync('dist/site/social-preview.png');assert.equal(png.readUInt32BE(16),1200);assert.equal(png.readUInt32BE(20),630);
for(const file of JSON.parse(read('pages-files.json')))assert(readFileSync(resolve('dist',file)).equals(readFileSync(resolve('../docs',file))),`Matching output ${file}`);
const cssPath=main.match(/href="(\/site\/build\/[^"]+\.css)"/)[1];
const css=read(resolve('dist',cssPath.slice(1)));
assert(/(?:[;{])backdrop-filter:blur\(/.test(css),'Standard backdrop blur survives CSS compilation');
assert(css.includes('-webkit-backdrop-filter:'),'Safari backdrop blur retained');
console.log(`${site.name}: anchors, verified formats, static content, metadata/CSP, local assets, retained endpoints and Pages output pass.`);
