import test from'node:test';
import assert from'node:assert/strict';
import{parseRelease}from'../src/core/releases.ts';
const good={tag_name:'v1.2.3',html_url:'https://github.com/emp0ry/Product/releases/tag/v1.2.3',assets:[{name:'Product.exe',state:'uploaded',size:1000,browser_download_url:'https://github.com/emp0ry/Product/releases/download/v1.2.3/Product.exe'}]};
test('official release assets resolve with exact repository, tag and positive size',()=>{const r=parseRelease(good,'Product');assert.equal(r?.assets['Product.exe'].size,1000);assert.equal(r?.assets['Product.exe'].url,good.assets[0].browser_download_url);});
test('a partial release remains useful without inventing missing formats',()=>{const r=parseRelease({...good,assets:[]},'Product');assert(r);assert.deepEqual(Object.keys(r.assets),[]);assert.equal(r.url,good.html_url);});
test('foreign URLs, credentials, drafts, prereleases and unfinished assets are rejected',()=>{
 for(const x of[{...good,html_url:good.html_url.replace('emp0ry','elsewhere')},{...good,draft:true},{...good,prerelease:true},{...good,tag_name:'../../other'}])assert.equal(parseRelease(x,'Product'),null);
 for(const url of['https://evil.example/file.exe','https://user:pass@github.com/emp0ry/Product/releases/download/v1.2.3/Product.exe',good.assets[0].browser_download_url.replace('v1.2.3','v1.0.0')])assert.deepEqual(Object.keys(parseRelease({...good,assets:[{...good.assets[0],browser_download_url:url}]},'Product')!.assets),[]);
 assert.deepEqual(Object.keys(parseRelease({...good,assets:[{...good.assets[0],state:'new'}]},'Product')!.assets),[]);
});
