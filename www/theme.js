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

    /* ── Dark-mode style injection ──────────────────────────────── */
    function applyDarkStyle() {
        if (document.getElementById('dark-override')) return;
        var css =
            ':root{' +
            '--bg-primary:#0b0b0d;--bg-secondary:#111214;' +
            '--text-primary:#e6e6e6;--text-secondary:#cfcfcf;}' +
            'body,header,footer,.brutalist-box,.image-placeholder{' +
            'background:var(--bg-primary)!important;color:var(--text-primary)!important;}' +
            'nav a{background:var(--bg-secondary)!important;color:var(--text-primary)!important;' +
            'border-color:var(--text-primary)!important;}' +
            '.login-container{background:var(--bg-secondary)!important;' +
            'border-color:var(--text-primary)!important;color:var(--text-primary)!important;}' +
            '.action-btn{background:var(--text-primary)!important;' +
            'color:var(--bg-secondary)!important;border-color:var(--text-primary)!important;}' +
            '.status-box{background:var(--bg-secondary)!important;' +
            'color:var(--text-primary)!important;border-color:var(--text-primary)!important;}' +
            '.cgi-result,.cgi-file-item{background:var(--bg-secondary)!important;' +
            'color:var(--text-primary)!important;border-color:var(--text-primary)!important;}';
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
            'font-weight:700;margin-left:12px;padding:0 8px;' +
            'border:2px solid currentColor;display:inline-flex;align-items:center;';
        userBadge.textContent = username.toUpperCase();
        nav.appendChild(userBadge);

        /* Show dark/light mode toggle only when logged in. */
        var toggle = document.createElement('button');
        toggle.id = 'theme-toggle';
        toggle.type = 'button';
        toggle.style.cssText = 'margin-left:8px;cursor:pointer;font-size:1rem;' +
            'background:transparent;border:none;padding:0 4px;';
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
