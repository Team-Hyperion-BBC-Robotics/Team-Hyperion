/**
 * Team Hyperion — Shared Nav & Footer Component
 * ─────────────────────────────────────────────
 * Include this script in every page (except emulator.html):
 *   <script src="assets/nav-component.js"></script>
 *
 * The script auto-detects the current page and marks the correct nav item active.
 * To update nav or footer across ALL pages, edit only this file.
 */

(function () {
  'use strict';

  /* ── NAV HTML ─────────────────────────────────────────────── */
  const NAV_HTML = `
<nav>
  <a href="index.html" class="nav-logo">
    <img src="assets/schoolLogo.png" alt="Brisbane Boys' College" class="nav-logo-img"/>
    <div class="nav-logo-text">TEAM <span>HYPERION</span></div>
  </a>

  <ul class="nav-links">
    <li><a href="index.html"        data-page="index">Home</a></li>
    <li><a href="team.html"         data-page="team">Team</a></li>
    <li><a href="robot.html"        data-page="robot">Robot</a></li>
    <li><a href="achievements.html" data-page="achievements">Achievements</a></li>
    <li><a href="logbook.html"      data-page="logbook">Logbook</a></li>

    <!-- Campaigns dropdown -->
    <li>
      <button class="nav-dropdown-trigger" onclick="HyperionNav.toggleDropdown('dd-campaigns', this)">
        Campaigns <span class="nav-chevron"></span>
      </button>
      <div class="nav-dropdown" id="dd-campaigns">
        <a href="campaign2025.html" data-page="campaign2025">📃 2025 Campaign — Salvador</a>
        <a href="campaign2026.html" data-page="campaign2026">📃 2026 Campaign — Incheon</a>
      </div>
    </li>

    <!-- Explore dropdown -->
    <li>
      <button class="nav-dropdown-trigger" onclick="HyperionNav.toggleDropdown('dd-explore', this)">
        Explore <span class="nav-chevron"></span>
      </button>
      <div class="nav-dropdown" id="dd-explore">
        <a href="resources.html" data-page="resources">📰 Resources</a>
        <a href="emulator.html"  data-page="emulator">📱 Robot Emulator</a>
      </div>
    </li>
  </ul>

  <a href="https://www.robocupjunior.org.au/" target="_blank" rel="noopener" style="text-decoration:none;">
    <div class="nav-badge">RoboCup Junior 🇦🇺</div>
  </a>

  <!-- Hamburger -->
  <button class="nav-hamburger" id="hyperion-hamburger" onclick="HyperionNav.toggleSidebar()" aria-label="Open menu">
    <span></span><span></span><span></span>
  </button>
</nav>

<!-- Mobile overlay -->
<div class="nav-mobile-overlay" id="hyperion-nav-overlay" onclick="HyperionNav.toggleSidebar()"></div>

<!-- Mobile sidebar -->
<div class="nav-sidebar" id="hyperion-nav-sidebar">
  <div class="nav-sidebar-section">
    <span class="nav-sidebar-label">Navigation</span>
    <a href="index.html"        data-page="index">Home</a>
    <a href="team.html"         data-page="team">Team</a>
    <a href="robot.html"        data-page="robot">Our Robot</a>
    <a href="achievements.html" data-page="achievements">Achievements</a>
    <a href="logbook.html"      data-page="logbook">📋 Engineering Logbook</a>
  </div>
  <div class="nav-sidebar-section">
    <span class="nav-sidebar-label">Campaigns</span>
    <a href="campaign2025.html" data-page="campaign2025">📃 2025 — Salvador, Brazil</a>
    <a href="campaign2026.html" data-page="campaign2026">📃 2026 — Incheon, South Korea</a>
  </div>
  <div class="nav-sidebar-section">
    <span class="nav-sidebar-label">Explore</span>
    <a href="resources.html" data-page="resources">📰 Resources</a>
    <a href="emulator.html"  data-page="emulator">📱 Robot Emulator</a>
  </div>
  <div style="padding:20px 28px;">
    <a href="https://www.robocupjunior.org.au/" target="_blank" rel="noopener"
       style="font-family:'Share Tech Mono',monospace;font-size:0.62rem;letter-spacing:0.1em;color:#00c85a;text-decoration:none;">
      RoboCup Junior Australia 🇦🇺 ↗
    </a>
  </div>
</div>`;

  /* ── FOOTER HTML ───────────────────────────────────────────── */
  const FOOTER_HTML = `
<footer>
  <div class="footer-social-bar">
    <a href="https://www.youtube.com/@hyperion-bbcrobotics" target="_blank" rel="noopener" class="footer-social-link">
      <span class="footer-social-icon">▶</span>YouTube
    </a>
    <a href="https://github.com/SamGargRobotics/Hyperion-BBC-Robotics" target="_blank" rel="noopener" class="footer-social-link">
      <span class="footer-social-icon">⌥</span>GitHub
    </a>
    <a href="mailto:bbchyperion@gmail.com" class="footer-social-link">
      <span class="footer-social-icon">✉</span>bbchyperion@gmail.com
    </a>
  </div>
  <div class="footer-legacy">
    <span class="footer-legacy-label">Legacy Sites →</span>
    <a href="https://sites.google.com/view/teamhyperionbbcrobotics/home" target="_blank" rel="noopener" class="footer-legacy-link">2024 Site</a>
    <span class="footer-legacy-sep">·</span>
    <a href="https://sites.google.com/view/team-hyperion-2025/home" target="_blank" rel="noopener" class="footer-legacy-link">2025 Site</a>
  </div>
  <div class="footer-main">
    <div class="footer-left">
      <img src="assets/schoolLogo.png" alt="BBC" class="footer-logo-img"/>
      <div class="footer-wordmark">TEAM HYPERION</div>
    </div>
    <div class="footer-center">
      © 2026 Team Hyperion · Brisbane Boys' College<br/>
      RoboCup Junior Australia · Lightweight Soccer Division
    </div>
    <div class="footer-right">
      <a href="achievements.html">Achievements</a> ·
      <a href="team.html">Team</a> ·
      <a href="robot.html">Robot</a> ·
      <a href="logbook.html">Logbook</a><br/>
      🇦🇺 Queensland, Australia
    </div>
  </div>
</footer>`;

  /* ── NAV CSS ───────────────────────────────────────────────── */
  const NAV_CSS = `
/* ═══════════════════════════════════════
   HYPERION SHARED NAV — injected by nav-component.js
   Edit nav-component.js to change nav/footer across all pages.
═══════════════════════════════════════ */
nav {
  position: fixed;
  top: 0; left: 0; right: 0;
  z-index: 1000;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0 32px;
  height: 64px;
  background: rgba(5,10,5,0.92);
  backdrop-filter: blur(14px);
  border-bottom: 1px solid rgba(0,114,63,0.2);
}
.nav-logo {
  display: flex; align-items: center; gap: 10px;
  text-decoration: none; flex-shrink: 0;
}
.nav-logo-img { height: 34px; width: auto; }
.nav-logo-text {
  font-family: 'Bebas Neue', sans-serif;
  font-size: 1.1rem; letter-spacing: 0.18em;
  color: rgba(240,255,244,0.85);
}
.nav-logo-text span { color: var(--green-bright, #00c85a); }

/* Desktop links */
.nav-links {
  display: flex; align-items: center;
  gap: 4px; list-style: none; margin: 0; padding: 0;
}
.nav-links > li { position: relative; }
.nav-links > li > a,
.nav-dropdown-trigger {
  display: flex; align-items: center; gap: 5px;
  padding: 8px 12px;
  font-family: 'Share Tech Mono', monospace;
  font-size: 0.7rem; letter-spacing: 0.12em; text-transform: uppercase;
  color: rgba(240,255,244,0.65);
  text-decoration: none; background: none; border: none;
  cursor: pointer; transition: color 0.2s; white-space: nowrap;
}
.nav-links > li > a:hover,
.nav-dropdown-trigger:hover,
.nav-links > li > a.active { color: var(--green-bright, #00c85a); }

.nav-chevron {
  width: 8px; height: 8px;
  border-right: 1.5px solid currentColor;
  border-bottom: 1.5px solid currentColor;
  transform: rotate(45deg) translateY(-2px);
  transition: transform 0.2s; flex-shrink: 0;
}
.nav-dropdown-trigger.open .nav-chevron {
  transform: rotate(-135deg) translateY(-2px);
}

/* Dropdown panel */
.nav-dropdown {
  position: absolute;
  top: calc(100% + 8px); left: 50%;
  min-width: 190px;
  background: rgba(8,16,8,0.97);
  border: 1px solid rgba(0,114,63,0.3);
  backdrop-filter: blur(16px);
  padding: 8px 0; opacity: 0; visibility: hidden; pointer-events: none;
  transform: translateX(-50%) translateY(-6px);
  transition: opacity 0.2s ease, transform 0.2s ease, visibility 0.2s;
}
.nav-dropdown.open {
  opacity: 1; visibility: visible; pointer-events: all;
  transform: translateX(-50%) translateY(0);
}
.nav-dropdown::before {
  content: ''; position: absolute; top: -5px; left: 50%;
  transform: translateX(-50%) rotate(45deg);
  width: 8px; height: 8px; background: rgba(8,16,8,0.97);
  border-left: 1px solid rgba(0,114,63,0.3);
  border-top: 1px solid rgba(0,114,63,0.3);
}
.nav-dropdown a {
  display: block; padding: 9px 18px;
  font-family: 'Share Tech Mono', monospace;
  font-size: 0.68rem; letter-spacing: 0.1em; text-transform: uppercase;
  color: rgba(240,255,244,0.6); text-decoration: none;
  transition: all 0.15s; border-left: 2px solid transparent;
}
.nav-dropdown a:hover,
.nav-dropdown a.active {
  color: var(--green-bright, #00c85a);
  border-left-color: var(--green-bright, #00c85a);
  background: rgba(0,114,63,0.08); padding-left: 22px;
}

/* Nav badge */
.nav-badge {
  font-family: 'Share Tech Mono', monospace;
  font-size: 0.62rem; letter-spacing: 0.1em;
  padding: 5px 10px; border: 1px solid rgba(0,114,63,0.35);
  color: var(--green-bright, #00c85a); white-space: nowrap; flex-shrink: 0;
}

/* Hamburger */
.nav-hamburger {
  display: none; flex-direction: column; justify-content: center;
  gap: 5px; width: 36px; height: 36px;
  background: none; border: none; cursor: pointer; padding: 4px; z-index: 1001;
}
.nav-hamburger span {
  display: block; height: 2px; background: rgba(240,255,244,0.8);
  border-radius: 2px; transition: all 0.3s ease; transform-origin: center;
}
.nav-hamburger.open span:nth-child(1) { transform: translateY(7px) rotate(45deg); }
.nav-hamburger.open span:nth-child(2) { opacity: 0; transform: scaleX(0); }
.nav-hamburger.open span:nth-child(3) { transform: translateY(-7px) rotate(-45deg); }

/* Mobile overlay */
.nav-mobile-overlay {
  display: none; position: fixed; inset: 0;
  background: rgba(0,0,0,0.6); z-index: 998;
  opacity: 0; transition: opacity 0.3s;
}
.nav-mobile-overlay.open { opacity: 1; }

/* Mobile sidebar */
.nav-sidebar {
  display: none; position: fixed; top: 0; right: 0;
  width: min(320px, 85vw); height: 100vh;
  background: rgba(6,12,6,0.98);
  border-left: 1px solid rgba(0,114,63,0.3);
  z-index: 999; transform: translateX(100%);
  transition: transform 0.35s cubic-bezier(0.4,0,0.2,1);
  overflow-y: auto; padding: 80px 0 40px; backdrop-filter: blur(20px);
}
.nav-sidebar.open { transform: translateX(0); }
.nav-sidebar-section { padding: 6px 0; border-bottom: 1px solid rgba(0,114,63,0.12); }
.nav-sidebar-label {
  display: block; padding: 10px 28px 6px;
  font-family: 'Share Tech Mono', monospace;
  font-size: 0.58rem; letter-spacing: 0.2em;
  color: rgba(0,200,90,0.45); text-transform: uppercase;
}
.nav-sidebar a {
  display: block; padding: 11px 28px;
  font-family: 'Share Tech Mono', monospace;
  font-size: 0.75rem; letter-spacing: 0.12em; text-transform: uppercase;
  color: rgba(240,255,244,0.7); text-decoration: none;
  transition: all 0.15s; border-left: 2px solid transparent;
}
.nav-sidebar a:hover,
.nav-sidebar a.active {
  color: #00c85a; border-left-color: #00c85a; background: rgba(0,114,63,0.07);
}

@media (max-width: 960px) {
  .nav-links, .nav-badge { display: none; }
  .nav-hamburger { display: flex; }
  .nav-mobile-overlay, .nav-sidebar { display: block; }
}`;

  /* ── INJECT CSS ────────────────────────────────────────────── */
  function injectStyles() {
    const style = document.createElement('style');
    style.id = 'hyperion-nav-styles';
    style.textContent = NAV_CSS;
    document.head.appendChild(style);
  }

  /* ── MARK ACTIVE PAGE ──────────────────────────────────────── */
  function markActivePage() {
    const path = window.location.pathname;
    const file = path.split('/').pop().replace('.html', '') || 'index';
    document.querySelectorAll('[data-page]').forEach(el => {
      el.classList.toggle('active', el.dataset.page === file);
    });
  }

  /* ── INJECT NAV ────────────────────────────────────────────── */
  function injectNav() {
    const existing = document.querySelector('nav');
    if (existing) existing.remove();

    const wrapper = document.createElement('div');
    wrapper.innerHTML = NAV_HTML.trim();
    const body = document.body;
    Array.from(wrapper.childNodes).reverse().forEach(node => {
      body.insertBefore(node, body.firstChild);
    });
  }

  /* ── INJECT FOOTER ─────────────────────────────────────────── */
  function injectFooter() {
    const existing = document.querySelector('footer');
    if (existing) existing.remove();

    const wrapper = document.createElement('div');
    wrapper.innerHTML = FOOTER_HTML.trim();
    document.body.appendChild(wrapper.firstElementChild);
  }

  /* ── DROPDOWN LOGIC ────────────────────────────────────────── */
  window.HyperionNav = {
    toggleDropdown(id, btn) {
      const dd = document.getElementById(id);
      document.querySelectorAll('.nav-dropdown').forEach(d => {
        if (d.id !== id) d.classList.remove('open');
      });
      document.querySelectorAll('.nav-dropdown-trigger').forEach(b => {
        if (b !== btn) b.classList.remove('open');
      });
      dd.classList.toggle('open');
      btn.classList.toggle('open');
    },
    toggleSidebar() {
      const sidebar  = document.getElementById('hyperion-nav-sidebar');
      const overlay  = document.getElementById('hyperion-nav-overlay');
      const burger   = document.getElementById('hyperion-hamburger');
      const open     = sidebar.classList.toggle('open');
      overlay.classList.toggle('open', open);
      burger.classList.toggle('open', open);
      document.body.style.overflow = open ? 'hidden' : '';
    }
  };

  /* ── CLOSE DROPDOWNS ON OUTSIDE CLICK ─────────────────────── */
  function bindCloseDropdowns() {
    document.addEventListener('click', e => {
      if (!e.target.closest('.nav-links > li')) {
        document.querySelectorAll('.nav-dropdown').forEach(d => d.classList.remove('open'));
        document.querySelectorAll('.nav-dropdown-trigger').forEach(b => b.classList.remove('open'));
      }
    });
  }

  /* ── INIT ──────────────────────────────────────────────────── */
  function init() {
    injectStyles();
    injectNav();
    injectFooter();
    markActivePage();
    bindCloseDropdowns();
  }

  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', init);
  } else {
    init();
  }
})();