export interface ReleaseAsset {url:string;size:number}
export interface Release {label:string;url:string;assets:Record<string,ReleaseAsset>}
export function parseRelease(value:unknown,repository:string):Release|null{
 if(!/^[A-Za-z0-9_.-]+$/.test(repository)||!value||typeof value!=='object')return null;
 const data=value as Record<string,unknown>;
 if(typeof data.tag_name!=='string'||!/^v?\d+(?:\.\d+){1,3}$/.test(data.tag_name)||data.draft===true||data.prerelease===true)return null;
 const prefix=`https://github.com/emp0ry/${repository}/releases`,url=`${prefix}/tag/${data.tag_name}`;
 if(data.html_url!==url)return null;
 const assets:Record<string,ReleaseAsset>=Object.create(null);
 if(Array.isArray(data.assets))for(const item of data.assets){
  if(!item||typeof item!=='object')continue;
  const asset=item as Record<string,unknown>;
  if(typeof asset.name!=='string'||!/^[A-Za-z0-9_.+-]+$/.test(asset.name)||asset.state!=='uploaded'||!Number.isSafeInteger(asset.size)||(asset.size as number)<=0)continue;
  const expected=`${prefix}/download/${data.tag_name}/${encodeURIComponent(asset.name)}`;
  if(asset.browser_download_url===expected)assets[asset.name]={url:expected,size:asset.size as number};
 }
 return {label:data.tag_name,url,assets};
}
export function mountRelease(root:HTMLElement,repository:string){
 const lifetime=new AbortController();let requested=false;
 async function resolve(){
  if(requested)return;requested=true;
  const status=root.querySelector<HTMLElement>('[data-release-status]')!;
  try{
   const response=await fetch(`https://api.github.com/repos/emp0ry/${repository}/releases/latest`,{headers:{Accept:'application/vnd.github+json'},signal:AbortSignal.any([lifetime.signal,AbortSignal.timeout(6500)])});
   if(!response.ok)throw new Error('Unavailable');
   const release=parseRelease(await response.json(),repository);if(!release)throw new Error('Invalid');
   if(lifetime.signal.aborted)return;
   root.querySelector('[data-release-label]')!.textContent=release.label;
   root.querySelectorAll<HTMLAnchorElement>('[data-asset]').forEach(link=>{
    const asset=release.assets[link.dataset.asset!];link.href=asset?.url??release.url;
    const size=link.closest('.download-option')?.querySelector('.asset-size');
    if(size)size.textContent=asset?`${(asset.size/1_000_000).toFixed(1)} MB · ${release.label}`:'Available formats on GitHub';
    const arrow=link.querySelector('[aria-hidden]');if(arrow)arrow.textContent=asset?'↓':'↗';
   });
   status.textContent='Latest stable release from the official repository.';root.dataset.release='resolved';
  }catch{if(!lifetime.signal.aborted){status.textContent='Live release check unavailable. Use the published downloads or release notes.';root.dataset.release='fallback';}}
 }
 const observer=new IntersectionObserver(entries=>{if(entries.some(e=>e.isIntersecting)){observer.disconnect();void resolve();}},{root:root.closest('.page-scroll'),rootMargin:'500px'});
 observer.observe(root);
 return {destroy(){lifetime.abort();observer.disconnect();}};
}
