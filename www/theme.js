(function () {
    /* ── Cookie helpers ─────────────────────────────────────────── */
    function readCookie(name) {
        var v = document.cookie.match('(?:^|; )' + name + '=([^;]*)');
        return v ? decodeURIComponent(v[1]) : null;
    }
    function setCookie(name, value, days) {
        var d = new Date();
        d.setTime(d.getTime() + days * 24 * 60 * 60 * 1000);
        document.cookie = name + '=' + encodeURIComponent(value) +
            '; path=/; expires=' + d.toUTCString();
    }
    function deleteCookie(name) {
        document.cookie = name + '=; Path=/; Max-Age=0';
    }

    /* ── Ayu Dark style injection ───────────────────────────────── */
    function applyDarkStyle() {
        if (document.getElementById('dark-override')) return;
        var css =
            /* Ayu Dark – override every CSS variable */
            ':root{' +
            '--bg-primary:#0a0e14;--bg-secondary:#0d1017;--bg-panel:#13181f;' +
            '--text-primary:#bfbdb6;--text-secondary:#8a9199;--text-muted:#626a73;' +
            '--border-color:#1a2332;' +
            '--border-thick:2px solid #1a2332;--border-thin:1px solid #1a2332;' +
            '--accent:#e6b450;--accent-alt:#39bae6;' +
            '--accent-green:#7fd962;--accent-red:#ff3333;' +
            '--shadow:0 2px 12px rgba(0,0,0,.45);' +
            '}' +
            'body,header,footer,.hero-section,.image-gallery{' +
            'background:var(--bg-primary)!important;color:var(--text-primary)!important;}' +
            '.brutalist-box,.status-box,.cgi-result,.cgi-file-item,' +
            '.login-container,.result,.page-header{' +
            'background:var(--bg-secondary)!important;' +
            'border-color:var(--border-color)!important;' +
            'color:var(--text-primary)!important;}' +
            '.form-group input,.form-group textarea,.form-group select{' +
            'background:var(--bg-panel)!important;color:var(--text-primary)!important;' +
            'border-color:var(--border-color)!important;}' +
            '.endpoint-item{background:var(--bg-secondary)!important;' +
            'color:var(--text-primary)!important;border-color:var(--border-color)!important;}' +
            '.endpoint-item:hover{background:var(--bg-panel)!important;' +
            'border-color:var(--accent)!important;}' +
            'nav a{color:var(--text-secondary)!important;}' +
            'nav a:hover{color:var(--accent)!important;background:var(--bg-panel)!important;}' +
            '.endpoint-path{color:var(--accent-alt)!important;}' +
            '.section-title{border-bottom-color:var(--accent)!important;}';
        var s = document.createElement('style');
        s.id = 'dark-override';
        s.appendChild(document.createTextNode(css));
        document.head.appendChild(s);
    }
    function removeDarkStyle() {
        var s = document.getElementById('dark-override');
        if (s) s.parentNode.removeChild(s);
    }

    /* ── Per-user theme persistence ─────────────────────────────── */
    function getUserTheme(user) {
        return readCookie('theme_' + user) || 'light';
    }
    function saveUserTheme(user, theme) {
        setCookie('theme_' + user, theme, 365);
    }

    /* ── Main init ──────────────────────────────────────────────── */
    function init() {
        var username = readCookie('username');
        var nav = document.querySelector('nav');

        /* Default: light mode always on every page load.
           If the user is logged in we override below. */
        removeDarkStyle();

        if (!username || !nav) {
            /* No logged-in user → stay in light mode, no toggle shown. */
            return;
        }

        /* ── User is logged in ── */

        /* Apply their saved theme preference. */
        if (getUserTheme(username) === 'dark') {
            applyDarkStyle();
        }

        /* Show username badge in nav. */
        var userBadge = document.createElement('span');
        userBadge.id = 'nav-user-badge';
        userBadge.style.cssText =
            'font-weight:600;margin-left:10px;padding:0.3rem 0.8rem;' +
            'border:1px solid;border-radius:4px;' +
            'display:inline-flex;align-items:center;font-size:0.72rem;letter-spacing:1px;';
        userBadge.textContent = username.toUpperCase();
        nav.appendChild(userBadge);

        /* Show dark/light mode toggle only when logged in. */
        var toggle = document.createElement('button');
        toggle.id = 'theme-toggle';
        toggle.type = 'button';
        toggle.style.cssText =
            'margin-left:6px;cursor:pointer;font-size:0.95rem;' +
            'background:transparent;border:none;padding:0.3rem 0.5rem;' +
            'border-radius:4px;line-height:1;';
        toggle.textContent = (getUserTheme(username) === 'dark') ? '\u2600\ufe0f' : '\ud83c\udf19';
        nav.appendChild(toggle);

        toggle.addEventListener('click', function () {
            var isDark = !!document.getElementById('dark-override');
            if (isDark) {
                removeDarkStyle();
                saveUserTheme(username, 'light');
                toggle.textContent = '\ud83c\udf19';
            } else {
                applyDarkStyle();
                saveUserTheme(username, 'dark');
                toggle.textContent = '\u2600\ufe0f';
            }
        });
    }

    if (document.readyState === 'loading')
        document.addEventListener('DOMContentLoaded', init);
    else
        init();
})();
