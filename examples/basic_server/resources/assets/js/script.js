/* Basic tests */
(() => {
  const $ = (s) => document.querySelector(s);

  // Fetch /version.json to verify static JSON serving and CORS
  const btn = $("#btn-fetch");
  const out = $("#fetch-result");
  if (btn && out) {
    btn.addEventListener("click", async () => {
      out.textContent = "実行中...";
      try {
        const res = await fetch("/version.json", { cache: "no-store" });
        out.textContent = res.ok
          ? JSON.stringify(await res.json(), null, 2)
          : `HTTP ${res.status}`;
      } catch (e) {
        out.textContent = String(e);
      }
    });
  }

  // Cache-control behavior check with cache-busting query
  const img = $("#cache-buster");
  const reload = $("#btn-reload-img");
  function setSrc() {
    const t = new Date().toISOString().replace(/[:.]/g,"");
    img.src = `/assets/img/sample.png?ts=${t}`;
  }
  if (img) setSrc();
  if (reload) reload.addEventListener("click", setSrc);
})();
