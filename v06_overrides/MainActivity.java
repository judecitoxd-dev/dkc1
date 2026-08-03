package com.yinative.port;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.webkit.JavascriptInterface;
import android.webkit.MimeTypeMap;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Toast;

import java.io.ByteArrayInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

public final class MainActivity extends Activity {
    private static final int REQUEST_ROM = 4101;
    private static final String PREFS = "yi_native_prefs";
    private static final String KEY_ROM_URI = "rom_uri";
    private static final String APP_HOST = "yi.local";
    private static final String START_URL = "https://" + APP_HOST + "/index.html";
    private static final String ROM_URL = "https://" + APP_HOST + "/rom/game.sfc";

    private WebView webView;
    private Uri romUri;
    private boolean pageReady;
    private boolean startupRomRequested;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(
                WindowManager.LayoutParams.FLAG_FULLSCREEN | WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
                WindowManager.LayoutParams.FLAG_FULLSCREEN | WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            WindowManager.LayoutParams attrs = getWindow().getAttributes();
            attrs.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
            getWindow().setAttributes(attrs);
        }
        hideSystemUi();

        SharedPreferences prefs = getSharedPreferences(PREFS, MODE_PRIVATE);
        String savedUri = prefs.getString(KEY_ROM_URI, null);
        if (savedUri != null) {
            try {
                romUri = Uri.parse(savedUri);
            } catch (RuntimeException ignored) {
                romUri = null;
            }
        }

        webView = new WebView(this);
        WebSettings settings = webView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setAllowFileAccess(false);
        settings.setAllowContentAccess(true);
        settings.setMediaPlaybackRequiresUserGesture(false);
        settings.setBuiltInZoomControls(false);
        settings.setDisplayZoomControls(false);
        settings.setSupportZoom(false);
        settings.setLoadWithOverviewMode(false);
        settings.setUseWideViewPort(false);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            settings.setSafeBrowsingEnabled(false);
        }
        webView.setBackgroundColor(0xFF000000);
        webView.setLayerType(View.LAYER_TYPE_HARDWARE, null);
        webView.addJavascriptInterface(new AndroidBridge(), "Android");
        webView.setWebViewClient(new AppWebViewClient());
        setContentView(webView);
        webView.loadUrl(START_URL);
    }

    @Override
    protected void onResume() {
        super.onResume();
        hideSystemUi();
        if (webView != null) {
            webView.onResume();
            webView.evaluateJavascript("window.onAndroidResume && window.onAndroidResume()", null);
        }
    }

    @Override
    protected void onPause() {
        if (webView != null) {
            webView.evaluateJavascript("window.onAndroidPause && window.onAndroidPause()", null);
            webView.onPause();
        }
        super.onPause();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) hideSystemUi();
    }

    private void hideSystemUi() {
        int flags = View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_STABLE;
        getWindow().getDecorView().setSystemUiVisibility(flags);
    }

    /**
     * Use the wildcard MIME type deliberately. Many Android file providers report .sfc/.smc as an
     * unknown MIME type, so application/octet-stream hides the ROM completely.
     */
    private void chooseRom() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("*/*");
        intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        intent.addFlags(Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION);
        intent.putExtra(Intent.EXTRA_MIME_TYPES, new String[]{
                "application/octet-stream",
                "application/x-snes-rom",
                "application/vnd.nintendo.snes.rom",
                "*/*"
        });
        intent.putExtra(Intent.EXTRA_TITLE, "Super Mario World 2 - Yoshi's Island (USA) (Rev 1).sfc");
        try {
            startActivityForResult(Intent.createChooser(intent, "Seleccionar ROM .sfc o .smc"), REQUEST_ROM);
        } catch (RuntimeException first) {
            Intent fallback = new Intent(Intent.ACTION_GET_CONTENT);
            fallback.addCategory(Intent.CATEGORY_OPENABLE);
            fallback.setType("*/*");
            fallback.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            startActivityForResult(Intent.createChooser(fallback, "Seleccionar ROM"), REQUEST_ROM);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode != REQUEST_ROM) return;
        if (resultCode != RESULT_OK || data == null || data.getData() == null) {
            notifyRomError("No se seleccionó ninguna ROM");
            return;
        }

        Uri selected = data.getData();
        try (InputStream check = getContentResolver().openInputStream(selected)) {
            if (check == null) throw new FileNotFoundException("El proveedor no devolvió datos");
            byte[] probe = new byte[64];
            if (check.read(probe) < 1) throw new IOException("El archivo está vacío");
        } catch (Exception ex) {
            notifyRomError("Android no pudo leer el archivo: " + ex.getMessage());
            return;
        }

        try {
            int flags = data.getFlags() & Intent.FLAG_GRANT_READ_URI_PERMISSION;
            if (flags != 0) getContentResolver().takePersistableUriPermission(selected, flags);
        } catch (RuntimeException ignored) {
            // Providers such as Downloads or some third-party file managers may
            // offer only a session permission. The selected ROM still loads now.
        }

        romUri = selected;
        getSharedPreferences(PREFS, MODE_PRIVATE).edit().putString(KEY_ROM_URI, selected.toString()).apply();
        requestRomLoad("selected");
    }

    private void requestRomLoad(String reason) {
        if (webView == null || romUri == null || !pageReady) return;
        String url = ROM_URL + "?reason=" + reason + "&t=" + System.nanoTime();
        webView.evaluateJavascript("window.loadRomFromUrl(" + jsQuote(url) + ")", null);
    }

    private static String jsQuote(String value) {
        return "\"" + value.replace("\\", "\\\\").replace("\"", "\\\"") + "\"";
    }

    private void notifyRomError(String message) {
        Toast.makeText(this, message, Toast.LENGTH_LONG).show();
        if (webView != null && pageReady) {
            webView.evaluateJavascript("window.onNativeRomError && window.onNativeRomError(" + jsQuote(message) + ")", null);
        }
    }

    private final class AndroidBridge {
        @JavascriptInterface
        public void chooseRom() {
            runOnUiThread(MainActivity.this::chooseRom);
        }

        @JavascriptInterface
        public boolean hasRom() {
            return romUri != null;
        }

        @JavascriptInterface
        public void loadSavedRom() {
            runOnUiThread(() -> requestRomLoad("saved"));
        }

        @JavascriptInterface
        public void clearSavedRom() {
            romUri = null;
            getSharedPreferences(PREFS, MODE_PRIVATE).edit().remove(KEY_ROM_URI).apply();
        }

        @JavascriptInterface
        public void toast(String message) {
            runOnUiThread(() -> Toast.makeText(MainActivity.this, message, Toast.LENGTH_SHORT).show());
        }
    }

    private final class AppWebViewClient extends WebViewClient {
        @Override
        public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
            Uri uri = request.getUrl();
            if (uri != null && APP_HOST.equals(uri.getHost())) return route(uri);
            return super.shouldInterceptRequest(view, request);
        }

        @Override
        @SuppressWarnings("deprecation")
        public WebResourceResponse shouldInterceptRequest(WebView view, String url) {
            try {
                Uri uri = Uri.parse(url);
                if (APP_HOST.equals(uri.getHost())) return route(uri);
            } catch (RuntimeException ignored) {
            }
            return super.shouldInterceptRequest(view, url);
        }

        @Override
        public void onPageFinished(WebView view, String url) {
            super.onPageFinished(view, url);
            hideSystemUi();
            if (url != null && url.startsWith(START_URL)) {
                pageReady = true;
                if (romUri != null && !startupRomRequested) {
                    startupRomRequested = true;
                    requestRomLoad("startup");
                }
            }
        }

        private WebResourceResponse route(Uri uri) {
            String path = uri.getPath();
            if (path == null || path.isEmpty() || "/".equals(path)) path = "/index.html";
            if ("/rom/game.sfc".equals(path)) return openRomResponse();
            return openAssetResponse(path.substring(1));
        }

        private WebResourceResponse openAssetResponse(String assetPath) {
            if (assetPath.contains("..")) return errorResponse(403, "Ruta no permitida");
            try {
                InputStream in = getAssets().open(assetPath);
                return response(mimeFor(assetPath), charsetFor(assetPath), 200, "OK", in);
            } catch (IOException ex) {
                return errorResponse(404, "Recurso no encontrado: " + assetPath);
            }
        }

        private WebResourceResponse openRomResponse() {
            if (romUri == null) return errorResponse(404, "ROM no seleccionada");
            try {
                InputStream in = getContentResolver().openInputStream(romUri);
                if (in == null) throw new FileNotFoundException("No se pudo abrir la URI");
                return response("application/octet-stream", null, 200, "OK", in);
            } catch (Exception ex) {
                return errorResponse(500, "No se pudo leer la ROM: " + ex.getMessage());
            }
        }

        private WebResourceResponse response(String mime, String charset, int status, String reason, InputStream in) {
            Map<String, String> headers = new HashMap<>();
            headers.put("Cache-Control", "no-store");
            headers.put("Access-Control-Allow-Origin", "https://" + APP_HOST);
            headers.put("X-Content-Type-Options", "nosniff");
            return new WebResourceResponse(mime, charset, status, reason, headers, in);
        }

        private WebResourceResponse errorResponse(int status, String text) {
            return response("text/plain", "UTF-8", status, text,
                    new ByteArrayInputStream(text.getBytes(StandardCharsets.UTF_8)));
        }

        private String mimeFor(String path) {
            String ext = MimeTypeMap.getFileExtensionFromUrl(path).toLowerCase(Locale.ROOT);
            String mime = MimeTypeMap.getSingleton().getMimeTypeFromExtension(ext);
            if (mime != null) return mime;
            if (path.endsWith(".js")) return "application/javascript";
            if (path.endsWith(".html")) return "text/html";
            if (path.endsWith(".json")) return "application/json";
            if (path.endsWith(".png")) return "image/png";
            return "application/octet-stream";
        }

        private String charsetFor(String path) {
            return path.endsWith(".html") || path.endsWith(".js") || path.endsWith(".json") ? "UTF-8" : null;
        }
    }
}
