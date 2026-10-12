const svg = document.getElementById('grid');
const nodePositions = [
  {x:90,y:190},{x:360,y:190},{x:235,y:65},{x:490,y:65},{x:575,y:285},{x:235,y:315},{x:490,y:315}
];
const nodeIcons = {'Substation':'⚡','Hospital':'✚','Water Treatment':'≋','Communications':'⌁','Consumer':'⌂'};
let state = null, selectedEdge = 1, repairsCompleted = 0, autoTimer = null, stormTimer = null;

// Public landmarks and approximate neighborhood coordinates for this educational demo.
// The model grid nodes are illustrative; all network connections are fictional.
const geoLocations = [
  {lat:12.9757,lng:77.5910,title:'Model Grid Node A — Central Bengaluru',place:'Central Bengaluru (approximate model point)',type:'Model grid node',icon:'⚡',model:true, note:'Illustrative node only; not a verified physical substation.'},
  {lat:13.02024,lng:77.59557,title:'Model Grid Node B — North Bengaluru',place:'North Bengaluru (approximate model point)',type:'Model grid node',icon:'⚡',model:true, note:'Illustrative node only; not a verified physical substation.'},
  {lat:12.963389,lng:77.5738194,title:'Victoria Hospital',place:'Fort Road, near City Market, Bengaluru',type:'Hospital',icon:'✚',note:'Public hospital location. Source: Bengaluru Urban district directory.'},
  {lat:12.92351,lng:77.59186,title:'Jayanagar Water Tank (BWSSB)',place:'Jayanagar, Bengaluru',type:'Water tank',icon:'▤',note:'Water tower location mapped in OpenStreetMap / Mapcarta.'},
  {lat:12.96931,lng:77.64734,title:'BSNL Telephone Exchange and CSC',place:'Indiranagar, Bengaluru',type:'Telecommunications',icon:'⌁',note:'Telephone exchange location mapped in OpenStreetMap / Mapcarta.'},
  {lat:12.91624,lng:77.60702,title:'BTM Water Tank',place:'BTM Layout, Bengaluru',type:'Water tank',icon:'▤',note:'Water tower location mapped in OpenStreetMap / Mapcarta.'},
  {lat:12.97595,lng:77.64004,title:'Indiranagar Water Tank / BWSSB waterworks area',place:'Indiranagar, Bengaluru',type:'Water infrastructure',icon:'▤',note:'OpenStreetMap maps a water tower and BWSSB waterworks in this area.'}
];
let realMap = null, realMapLayers = null;
function initRealMap() {
  if (!window.L || !$('bengaluruMap')) {
    $('bengaluruMap').innerHTML = '<div style="padding:20px">Map library could not load. Check your internet connection and reload this page.</div>';
    return;
  }
  realMap = L.map('bengaluruMap', {scrollWheelZoom:false}).setView([12.9635,77.605], 12);
  L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    maxZoom:19,
    attribution:'&copy; <a href="https://www.openstreetmap.org/copyright" target="_blank" rel="noopener">OpenStreetMap contributors</a>'
  }).addTo(realMap);
  realMapLayers = L.layerGroup().addTo(realMap);
  const bounds = geoLocations.map(p=>[p.lat,p.lng]);
  realMap.fitBounds(bounds, {padding:[28,28]});
  setTimeout(()=>realMap.invalidateSize(),250);
}
function drawRealMap() {
  if (!realMap || !realMapLayers || !state) return;
  realMapLayers.clearLayers();
  state.edges.forEach(e=>{
    const a=geoLocations[e.u], b=geoLocations[e.v];
    if(!a||!b) return;
    const repaired=e.status==='repaired';
    const line=L.polyline([[a.lat,a.lng],[b.lat,b.lng]], {
      color:repaired?'#16875e':'#d34e5d', weight:repaired?5:4, opacity:.88,
      dashArray:repaired?null:'8 8'
    }).addTo(realMapLayers);
    line.bindTooltip(`Model line e${e.id} · ${repaired?'restored':'damaged'} · effort ${e.weight}`);
    line.on('click',()=>selectEdge(e.id));
  });
  state.nodes.forEach(n=>{
    const g=geoLocations[n.id]; if(!g) return;
    const marker=L.marker([g.lat,g.lng],{icon:L.divIcon({className:'',html:`<div class="map-marker ${g.model?'model':(n.powered?'powered':'unpowered')}">${g.icon}</div>`,iconSize:[32,32],iconAnchor:[16,16]})}).addTo(realMapLayers);
    const status=n.powered?'POWERED':'NO POWER';
    marker.bindPopup(`<div class="map-popup-title">${esc(g.title)}</div><div class="map-popup-type">${esc(g.type)} · ${esc(g.place)}</div><div class="map-popup-status" style="color:${g.model?'#4d6b8a':(n.powered?'#16875e':'#d34e5d')}">${g.model?'MODEL NODE':status}</div><p>${esc(g.note)}</p>`);
    marker.bindTooltip(g.title,{direction:'top',offset:[0,-14],className:'map-label'});
  });
}

const $ = id => document.getElementById(id);
const esc = s => String(s).replace(/[&<>"']/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
async function api(path, method='GET') {
  const res = await fetch(path, {method});
  const data = await res.json();
  if (!res.ok || data.error || data.ok === false) throw new Error(data.error || 'Request failed');
  return data;
}
function log(title, detail) {
  const li = document.createElement('li');
  li.innerHTML = `<span class="event-dot"></span><div><strong>${esc(title)}</strong><small>${esc(detail)}</small></div><time>${new Date().toLocaleTimeString([], {hour:'2-digit',minute:'2-digit'})}</time>`;
  $('eventLog').prepend(li);
}
function drawGrid() {
  if (!state) return;
  let out = '';
  state.edges.forEach(e => {
    const a=nodePositions[e.u], b=nodePositions[e.v], midX=(a.x+b.x)/2, midY=(a.y+b.y)/2;
    out += `<line class="edge ${e.status} ${selectedEdge===e.id?'selected':''}" x1="${a.x}" y1="${a.y}" x2="${b.x}" y2="${b.y}" data-edge="${e.id}"/>`;
    out += `<g pointer-events="none"><rect x="${midX-21}" y="${midY-12}" width="42" height="22" rx="7" fill="#fff" stroke="#e4e9f1"/><text x="${midX}" y="${midY+3}" font-size="10" font-weight="800" fill="${e.status==='repaired'?'#16875e':'#d34e5d'}" text-anchor="middle">e${e.id} · ${e.weight}</text></g>`;
  });
  state.nodes.forEach(n => {
    const p=nodePositions[n.id], title=n.name.replace('Residential ','Home ');
    const width = n.name.length > 14 ? 136 : 122;
    const icon = nodeIcons[n.type] || '⚡';
    out += `<g class="node ${n.powered?'node-powered':'node-no-power'}" data-node="${n.id}" transform="translate(${p.x-width/2},${p.y-29})">
      <rect class="node-card" width="${width}" height="58" rx="10"/>
      <text x="${width/2}" y="17" font-size="12" fill="${n.powered?'#16875e':'#cf4b5a'}" text-anchor="middle">${icon}</text>
      <text class="node-label" x="${width/2}" y="32">${esc(title)}</text>
      <text class="node-status" x="${width/2}" y="46" fill="${n.powered?'#16875e':'#cf4b5a'}">${n.powered?'● POWERED':'● NO POWER'}</text></g>`;
  });
  svg.innerHTML=out;
  svg.querySelectorAll('[data-edge]').forEach(el=>el.addEventListener('click',()=>selectEdge(Number(el.dataset.edge))));
  svg.querySelectorAll('[data-node]').forEach(el=>el.addEventListener('click',()=> {
    const n=Number(el.dataset.node);
    const e=state.edges.find(x=>x.u===n||x.v===n);
    if(e) selectEdge(e.id);
  }));
}
function selectEdge(id) {
  selectedEdge=id;
  $('edgeSelect').value=String(id);
  drawGrid();
}
function render() {
  if(!state) return;
  drawGrid();
  drawRealMap();
  const powered=state.nodes.filter(n=>n.powered).length;
  const damaged=state.edges.filter(e=>e.status==='damaged').length;
  $('poweredCount').textContent=`${powered} / ${state.nodes.length}`;
  $('criticalCount').textContent=state.criticalWithoutPower;
  $('damagedCount').textContent=damaged;
  $('repairsCount').textContent=repairsCompleted;
  $('priorityCost').textContent=state.scheduleEffort;
  $('mstCost').textContent=state.minimumNetworkEffort;
  const max=Math.max(state.scheduleEffort,state.minimumNetworkEffort,1);
  $('priorityBar').style.width=(state.scheduleEffort/max*100)+'%';
  $('mstBar').style.width=(state.minimumNetworkEffort/max*100)+'%';
  const previous=$('edgeSelect').value;
  $('edgeSelect').innerHTML=state.edges.map(e=>`<option value="${e.id}">e${e.id}: ${esc(state.nodes[e.u].name)} — ${esc(state.nodes[e.v].name)} (${e.weight} effort · ${e.status})</option>`).join('');
  if(state.edges.some(e=>String(e.id)===previous)) $('edgeSelect').value=previous;
  else $('edgeSelect').value=String(selectedEdge);
  $('schedule').innerHTML=state.schedule.map((id,i)=>{
    const e=state.edges[id], label=`${state.nodes[e.u].name} → ${state.nodes[e.v].name}`;
    return `<li class="${i===0?'next':''}">${esc(label)}<small>Line e${id} · effort ${e.weight}${i===0?' · NEXT PRIORITY':''}</small></li>`;
  }).join('') || '<li>All useful repairs completed.</li>';
  $('locationList').innerHTML=state.nodes.map(n=>{
    const critical=['Hospital','Water Treatment','Communications'].includes(n.type);
    return `<div class="location"><strong>${esc(n.name)}</strong><small>${esc(n.type)}${critical?' · Critical service':''}</small><span class="pill ${n.powered?'on':'offline'}">${n.powered?'POWERED':'NO POWER'}</span>${critical?'<span class="pill critical">PRIORITY</span>':''}</div>`;
  }).join('');
  $('criticalTag').textContent=`${state.criticalWithoutPower} critical offline`;
  $('stepBtn').disabled=state.schedule.length===0;
  $('autoToggle').disabled=state.schedule.length===0 && !autoTimer;
}
async function refresh(){ state=await api('/api/state'); render(); }
async function act(path, label, detail) {
  try {
    const before=state?state.edges.map(e=>e.status):[];
    state=await api(path,'POST');
    if(label==='Repair' || label==='Automatic repair') {
      const changed=state.edges.filter((e,i)=>before[i]==='damaged'&&e.status==='repaired').length;
      repairsCompleted+=changed;
    }
    render(); log(label,detail);
  } catch(e) { log('Action failed',e.message); alert(e.message); }
}
$('edgeSelect').addEventListener('change',e=>selectEdge(Number(e.target.value)));
$('repairBtn').addEventListener('click',()=>act(`/api/repair?edge=${selectedEdge}`,'Repair',`Repaired line e${selectedEdge}.`));
$('damageBtn').addEventListener('click',()=>act(`/api/damage?edge=${selectedEdge}`,'New damage',`Line e${selectedEdge} marked damaged.`));
$('stepBtn').addEventListener('click',async()=>{
  const next=state.schedule[0];
  if(next===undefined) return;
  const edge=state.edges[next];
  await act('/api/step','Priority repair',`Restored line e${next}: ${state.nodes[edge.u].name} — ${state.nodes[edge.v].name}.`);
});
$('stormBtn').addEventListener('click',async()=>{
  stopAuto();
  $('stormOverlay').classList.add('show');
  setTimeout(()=>$('stormOverlay').classList.remove('show'),1500);
  await act('/api/reset','Storm simulation','Storm event applied: all power lines are damaged.');
  repairsCompleted=0; render();
  log('Storm warning','All seven lines reset to damaged status.');
});
$('resetBtn').addEventListener('click',async()=>{
  stopAuto(); repairsCompleted=0;
  await act('/api/reset','Scenario reset','Returned to the initial storm scenario.');
});
function stopAuto(){if(autoTimer){clearInterval(autoTimer);autoTimer=null;} $('autoToggle').checked=false;}
$('autoToggle').addEventListener('change',e=>{
  if(e.target.checked){
    log('Automatic mode','Priority repairs will run every 1.8 seconds.');
    autoTimer=setInterval(async()=>{
      if(!state || state.schedule.length===0){stopAuto();log('Restoration complete','No further useful repairs are available.');return;}
      if(!window.actionBusy){
        window.actionBusy=true;
        try{const next=state.schedule[0];const edge=state.edges[next];await act('/api/step','Automatic repair',`Repaired e${next}: ${edge.weight} effort units.`);}
        finally{window.actionBusy=false;}
      }
    },1800);
  } else stopAuto();
});
$('clearLog').addEventListener('click',()=>$('eventLog').innerHTML='');
initRealMap();
refresh().then(()=>log('Engine online','Loaded C++ graph, BFS, Dijkstra, Kruskal and repair scheduler.')).catch(e=>{alert('Could not connect to the C++ engine. Run start.bat first. '+e.message);});
