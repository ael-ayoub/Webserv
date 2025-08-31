<?php
// Mandatory CGI header
header("Content-Type: text/html; charset=utf-8");

/*
 * photo_info.php — CGI-safe version
 * Generates (or embeds) a photo and shows its metadata in one file.
 */

$title       = "Sample Photo";
$description = "Auto-generated demo image (one-file PHP).";
$photographer= "Your Name";
$license     = "CC BY 4.0";
$takenOn     = date("F j, Y");

// Try to generate a JPEG using GD (preferred)
$binary = null;
$mime   = null;

if (function_exists('imagecreatetruecolor') && function_exists('imagejpeg')) {
    $w = 960; $h = 540;
    $im = imagecreatetruecolor($w, $h);

    // Gradient background
    for ($y = 0; $y < $h; $y++) {
        $r = 40 + (int)(215 * $y / $h);
        $g = 70 + (int)(165 * $y / $h);
        $b = 90 + (int)(125 * $y / $h);
        $col = imagecolorallocate($im, $r, $g, $b);
        imageline($im, 0, $y, $w, $y, $col);
    }

    // Overlay bar
    $overlay = imagecolorallocatealpha($im, 255, 255, 255, 60);
    imagefilledrectangle($im, 24, 24, $w - 24, 120, $overlay);

    // Text (built-in fonts so no TTF dependency)
    $txt = imagecolorallocate($im, 20, 20, 20);
    imagestring($im, 5, 36, 42, $title, $txt);
    imagestring($im, 3, 36, 76, $description, $txt);

    // Encode to JPEG
    ob_start();
    imagejpeg($im, null, 85);
    imagedestroy($im);
    $binary = ob_get_clean();
    $mime   = 'image/jpeg';
} 

// Fallback: small embedded PNG if GD not available
if ($binary === null) {
    $fallbackBase64 = 'iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR4nGNgYGBgAAAABAABJzQnWQAAAABJRU5ErkJggg==';
    $binary = base64_decode($fallbackBase64);
    $mime   = 'image/png';
}

// Probe image info
$info = @getimagesizefromstring($binary);
if (is_array($info) && isset($info['mime'])) {
    $mime = $info['mime'];
    $imgW = $info[0] ?? null;
    $imgH = $info[1] ?? null;
} else {
    $imgW = null; 
    $imgH = null;
}

// Aspect ratio helper
function gcd_int($a, $b) { return $b ? gcd_int($b, $a % $b) : $a; }
$aspect = 'Unknown';
if ($imgW && $imgH) {
    $g = gcd_int($imgW, $imgH);
    if ($g > 0) $aspect = ($imgW / $g) . ':' . ($imgH / $g);
}

$filesizeBytes = strlen($binary);
$filesizeHuman = ($filesizeBytes >= 1024) 
    ? number_format($filesizeBytes / 1024, 2) . ' KB'
    : $filesizeBytes . ' B';
$hash = hash('sha256', $binary);
$base64 = base64_encode($binary);
?>
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>Photo & Info — One File PHP (CGI)</title>
<meta name="viewport" content="width=device-width,initial-scale=1">
<style>
    body { font-family: system-ui, sans-serif; background: #0b1020; color: #e6ecff; margin: 0; }
    .wrap { max-width: 900px; margin: 32px auto; padding: 16px; }
    .card { background: #121a2f; border-radius: 16px; overflow: hidden; }
    .media { width: 100%; height: auto; display: block; }
    .content { padding: 20px; }
    h1 { margin: 0 0 8px; font-size: 24px; }
    .sub { color: #8ea0c6; margin: 0 0 18px; }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(220px, 1fr)); gap: 12px; }
    .kv { padding: 12px; background: #0f1830; border-radius: 12px; }
    .k { font-size: 12px; color: #8ea0c6; text-transform: uppercase; }
    .v { font-size: 14px; margin-top: 4px; word-break: break-all; }
    .badge { padding: 4px 10px; border-radius: 999px; background: #0e1731; border: 1px solid #1f2f57; color: #7aa2ff; }
    .footer { margin-top: 18px; font-size: 12px; color: #8ea0c6; }
</style>
</head>
<body>
<div class="wrap">
    <div class="card">
        <img class="media" src="data:<?php echo htmlspecialchars($mime, ENT_QUOTES); ?>;base64,<?php echo $base64; ?>" alt="<?php echo htmlspecialchars($title, ENT_QUOTES); ?>">
        <div class="content">
            <h1><?php echo htmlspecialchars($title, ENT_QUOTES); ?></h1>
            <p class="sub"><?php echo htmlspecialchars($description, ENT_QUOTES); ?></p>

            <div class="grid">
                <div class="kv"><div class="k">Photographer</div><div class="v"><?php echo htmlspecialchars($photographer, ENT_QUOTES); ?></div></div>
                <div class="kv"><div class="k">Taken on</div><div class="v"><?php echo htmlspecialchars($takenOn, ENT_QUOTES); ?></div></div>
                <div class="kv"><div class="k">MIME Type</div><div class="v"><?php echo htmlspecialchars($mime, ENT_QUOTES); ?></div></div>
                <div class="kv"><div class="k">Dimensions</div><div class="v"><?php echo ($imgW && $imgH) ? "{$imgW} × {$imgH}px" : "Unknown"; ?></div></div>
                <div class="kv"><div class="k">Aspect ratio</div><div class="v"><?php echo $aspect; ?></div></div>
                <div class="kv"><div class="k">File size</div><div class="v"><?php echo $filesizeHuman; ?></div></div>
                <div class="kv"><div class="k">SHA-256</div><div class="v"><code><?php echo $hash; ?></code></div></div>
                <div class="kv"><div class="k">License</div><div class="v"><span class="badge"><?php echo htmlspecialchars($license, ENT_QUOTES); ?></span></div></div>
            </div>

            <div class="footer">
                This page was executed as CGI/PHP with headers explicitly set.
            </div>
        </div>
    </div>
</div>
</body>
</html>
