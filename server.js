
const http = require('http');
const fs = require('fs');
const path = require('path');
const { spawn } = require('child_process');
const readline = require('readline');

const ROOT = __dirname;
const WEB = path.join(ROOT, 'web');
const exe = path.join(ROOT, process.platform === 'win32' ? 'grid-bridge.exe' : 'grid-bridge');
if (!fs.existsSync(exe)) {
  console.error('Missing C++ bridge. Run build.bat first.');
  process.exit(1);
}
const engine = spawn(exe, [], { cwd: ROOT, stdio: ['pipe', 'pipe', 'inherit'] });
const rl = readline.createInterface({ input: engine.stdout });
const pending = [];
rl.on('line', line => {
  const job = pending.shift();
  if (job) {
    try { job.resolve(JSON.parse(line)); }
    catch (e) { job.reject(new Error('Invalid engine response: ' + line)); }
  }
});
engine.on('error', err => console.error('C++ engine error:', err.message));
function command(text) {
  return new Promise((resolve, reject) => {
    pending.push({resolve, reject});
    engine.stdin.write(text + '\n');
    setTimeout(() => {
      const index = pending.findIndex(x => x.resolve === resolve);
      if (index >= 0) { pending.splice(index, 1); reject(new Error('C++ engine timed out')); }
    }, 5000).unref();
  });
}
const types = { '.html':'text/html; charset=utf-8', '.css':'text/css; charset=utf-8', '.js':'text/javascript; charset=utf-8' };
const server = http.createServer(async (req, res) => {
  const url = new URL(req.url, 'http://localhost');
  if (url.pathname.startsWith('/api/')) {
    try {
      let result;
      if (url.pathname === '/api/state') result = await command('state');
      else if (url.pathname === '/api/reset' && req.method === 'POST') result = await command('reset');
      else if (url.pathname === '/api/repair' && req.method === 'POST') result = await command('repair ' + Number(url.searchParams.get('edge')));
      else if (url.pathname === '/api/damage' && req.method === 'POST') result = await command('damage ' + Number(url.searchParams.get('edge')));
      else if (url.pathname === '/api/step' && req.method === 'POST') result = await command('step');
      else { res.writeHead(404); res.end(JSON.stringify({error:'Not found'})); return; }
      res.writeHead(result.ok === false ? 400 : 200, {'Content-Type':'application/json; charset=utf-8','Cache-Control':'no-store'});
      res.end(JSON.stringify(result));
    } catch (e) {
      res.writeHead(500, {'Content-Type':'application/json'}); res.end(JSON.stringify({error:e.message}));
    }
    return;
  }
  const pathname = url.pathname === '/' ? '/index.html' : decodeURIComponent(url.pathname);
  const full = path.normalize(path.join(WEB, pathname));
  if (!full.startsWith(WEB + path.sep) && full !== path.join(WEB,'index.html')) { res.writeHead(403); res.end('Forbidden'); return; }
  fs.readFile(full, (err, data) => {
    if (err) { res.writeHead(404); res.end('Not found'); return; }
    res.writeHead(200, {'Content-Type':types[path.extname(full)] || 'application/octet-stream'});
    res.end(data);
  });
});
server.listen(4173, '127.0.0.1', () => console.log('Power Grid Simulation: http://127.0.0.1:4173'));
function shutdown(){ engine.stdin.end('quit\n'); server.close(); }
process.on('SIGINT', shutdown);
process.on('SIGTERM', shutdown);
