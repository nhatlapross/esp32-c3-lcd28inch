#include <lvgl.h>
#include "lv_conf.h"
#include <demos/lv_demos.h>
#include "LCD_1in28.h"
#include "DEV_Config.h"
#include "CST816S.h"
#include "ui.h"  // SquareLine Studio UI
#include <WiFi.h>
#include <WiFiMulti.h>  // Required by MicroSui library
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <MicroSui.h>
#include "QMI8658.h"  // IMU sensor for step counting

// WiFiMulti object is defined in MicroSui library, just declare it here
extern WiFiMulti WiFiMulti;

#define EXAMPLE_LVGL_TICK_PERIOD_MS    2

// WiFi Configuration - Can be updated from Screen4
char WIFI_SSID[33] = "XuanHop_2.4G";  // Max 32 chars + null terminator
char WIFI_PASSWORD[65] = "Hongnhung@24031980";  // Max 64 chars + null terminator

// SUI Configuration
const char* SUI_RPC_URL = "https://fullnode.testnet.sui.io:443/";
const char* SUI_ADDRESS = "0xb0bd39aa7e029a1c61584fded4c78529eced33d3e43ef54adf50b1e151efefe3";  // Replace with your SUI address

// Transaction Builder Server Configuration
// Run: cd tx-server && node server.mjs
// Change this to your computer's IP address on local network
const char* TX_SERVER_URL = "http://192.168.1.11:3000";  // Server IP address

// SUI Private Key (Bech32 format) - KEEP THIS SECRET!
// Replace with your actual private key for signing transactions
const char* SUI_PRIVATE_KEY = "suiprivkey1qr9h2z0gxxqnrau67slz3tlct76xvr9m05734he24wjev23pe8mvuvjytq7";

// Prebuilt Transaction Bytes (generated for your address)
// Transaction: Split 0.001 SUI and transfer to self (test transaction)
// Sender: 0xb0bd39aa7e029a1c61584fded4c78529eced33d3e43ef54adf50b1e151efefe3
// Network: Testnet
const char* TRANSACTION_BYTES = "000002000840420f00000000000020b0bd39aa7e029a1c61584fded4c78529eced33d3e43ef54adf50b1e151efefe30202000101000001010300000000010100b0bd39aa7e029a1c61584fded4c78529eced33d3e43ef54adf50b1e151efefe30258b151671b338b72c7d4bf9ea5fcd6a801e12d2d5eac4d034eda52375e67f751452a0225000000002095c2dce66022e6666b6f9289dbc6be37d2e6fb4d415ad27ab960bb9beea16f61ea07f9ee01f9848d7d5367ae9d57cede9b64e377cd4e2b5fe80d757c4d48df7c452a022500000000203414d3059fab242bdc052eabc77876ab11d595c0936759a20a4d56efee107413b0bd39aa7e029a1c61584fded4c78529eced33d3e43ef54adf50b1e151efefe3e80300000000000040ab3c000000000000";

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

CST816S touch(6, 7, 13, 5);  // sda, scl, rst, irq

// Buffer for custom LCD driver (allocated in PSRAM)
// This is required by LCD_1in28.cpp
UWORD *BlackImage = NULL;

// Countdown timer variables
int countdown_value = 60;
unsigned long last_countdown_update = 0;
const unsigned long countdown_interval = 1000; // 1 second

// Balance update variables
unsigned long last_balance_update = 0;
const unsigned long balance_update_interval = 30000; // 30 seconds
String current_balance = "0";

// SUI Client and Keypair
MicroSuiClient suiClient;
MicroSuiEd25519 suiKeypair;
bool suiInitialized = false;

// Flag to indicate if transaction is in progress
volatile bool transactionInProgress = false;

// Task handle for transaction
TaskHandle_t transactionTaskHandle = NULL;

// Status screen objects
lv_obj_t * statusScreen = NULL;
lv_obj_t * statusLabel = NULL;
lv_obj_t * statusIcon = NULL;
lv_obj_t * statusMessage = NULL;

// Step counter variables
int stepCount = 0;
float lastAccMagnitude = 0;
bool stepDetected = false;
unsigned long lastStepTime = 0;
const unsigned long stepCooldown = 300; // 300ms between steps
const float stepThreshold = 0.8; // g-force threshold for step detection
bool imuInitialized = false;

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing using custom LCD driver */
void my_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    // Copy LVGL buffer to BlackImage at correct position (full-screen buffer layout)
    // BlackImage is 240x240, copy the dirty region into it
    uint16_t *src = (uint16_t *)&color_p->full;

    for(uint32_t row = 0; row < h; row++) {
        uint32_t y = area->y1 + row;
        uint32_t dst_offset = area->x1 + y * 240;  // BlackImage is 240 width
        uint16_t *src_row = &src[row * w];

        // Copy and swap bytes for this row
        for(uint32_t col = 0; col < w; col++) {
            uint16_t color = src_row[col];
            BlackImage[dst_offset + col] = (color >> 8) | (color << 8);
        }
    }

    // Use original LCD_1IN28_DisplayWindows function
    // Note: LVGL area is inclusive (x2, y2 are last pixels)
    // But DisplayWindows expects exclusive end (so +1)
    LCD_1IN28_DisplayWindows(area->x1, area->y1, area->x2 + 1, area->y2 + 1, BlackImage);

    lv_disp_flush_ready( disp_drv );
}

void example_increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

/*Read the touchpad - FIXED VERSION*/
void my_touchpad_read( lv_indev_drv_t * indev_drv, lv_indev_data_t * data )
{
    // Read touch input
    if( touch.available() )
    {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch.data.x;
        data->point.y = touch.data.y;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

// Shorten SUI address for display (e.g., 0xb0bd39..efe3)
String shortenAddress(const char* address) {
    String addr = String(address);
    if (addr.length() > 16) {
        String prefix = addr.substring(0, 4);   // First 8 characters (0xb0bd39)
        String suffix = addr.substring(addr.length() - 4);  // Last 4 characters
        return prefix + ".." + suffix;
    }
    return addr;  // Return as-is if too short
}

// WiFi auto-connect function
void connectWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.mode(WIFI_STA);

    // Disable WiFi sleep mode to prevent disconnection during operations
    WiFi.setSleep(false);

    // Setup WiFiMulti (required by MicroSui library)
    WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

    // Also connect using standard WiFi for our own use
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        // Set WiFi to maximum power to maintain stable connection
        WiFi.setTxPower(WIFI_POWER_19_5dBm);

        // Verify WiFiMulti is also connected
        Serial.print("WiFiMulti status: ");
        Serial.println(WiFiMulti.run() == WL_CONNECTED ? "Connected" : "Not Connected");
    } else {
        Serial.println("\nWiFi connection failed!");
    }
}

// Forward declarations for screen functions
void showProgressScreen(const char* title, const char* message);
void updateProgressMessage(const char* message);
void showStatusScreen(bool success, const char* title, const char* message, int autoCloseMs);

// Reconnect WiFi with new credentials (called from Screen4)
extern "C" void reconnectWiFiWithCredentials(const char* ssid, const char* password) {
    Serial.println("\n========================================");
    Serial.println("Reconnecting WiFi with new credentials...");
    Serial.println("========================================");

    // Show progress
    showProgressScreen("WiFi", "Connecting...");

    // Update global credentials
    strncpy(WIFI_SSID, ssid, sizeof(WIFI_SSID) - 1);
    WIFI_SSID[sizeof(WIFI_SSID) - 1] = '\0';

    strncpy(WIFI_PASSWORD, password, sizeof(WIFI_PASSWORD) - 1);
    WIFI_PASSWORD[sizeof(WIFI_PASSWORD) - 1] = '\0';

    Serial.print("New SSID: ");
    Serial.println(WIFI_SSID);
    Serial.println("New Password: ******");

    // Disconnect current WiFi
    updateProgressMessage("Disconnecting...");
    WiFi.disconnect();
    delay(500);

    // Reconnect with new credentials
    String connectMsg = "Connecting to\n" + String(ssid);
    updateProgressMessage(connectMsg.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    // Update WiFiMulti
    WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✓ WiFi connected successfully!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        WiFi.setTxPower(WIFI_POWER_19_5dBm);

        // Show success
        String successMsg = "Connected!\nIP: " + WiFi.localIP().toString();
        showStatusScreen(true, "WiFi OK", successMsg.c_str(), 3000);
    } else {
        Serial.println("\n✗ WiFi connection failed!");

        // Show failure
        showStatusScreen(false, "WiFi Failed", "Could not connect\nCheck credentials", 3000);
    }

    Serial.println("========================================\n");
}

// Fetch fresh transaction bytes from server
String fetchTransactionFromServer(int amount = 1000000) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected!");
        return "";
    }

    HTTPClient http;
    String url = String(TX_SERVER_URL) + "/build-transaction?amount=" + String(amount);

    Serial.println("Fetching fresh transaction from server...");
    Serial.println("URL: " + url);

    http.begin(url);
    http.setTimeout(10000); // 10 second timeout

    int httpCode = http.GET();

    String txBytes = "";
    if (httpCode == 200) {
        String response = http.getString();
        Serial.println("Server response received");

        // Parse JSON response
        StaticJsonDocument<2048> doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error && doc["success"] == true) {
            txBytes = doc["txBytes"].as<String>();
            Serial.println("✓ Transaction bytes fetched successfully!");
            Serial.print("  Length: ");
            Serial.print(txBytes.length() / 2);
            Serial.println(" bytes");

            if (doc.containsKey("details")) {
                Serial.print("  Amount: ");
                Serial.print(doc["details"]["amountSUI"].as<String>());
                Serial.println(" SUI");
            }
        } else {
            Serial.println("JSON parse error or server returned failure!");
            if (doc.containsKey("error")) {
                Serial.print("  Error: ");
                Serial.println(doc["error"].as<String>());
            }
        }
    } else {
        Serial.print("HTTP error code: ");
        Serial.println(httpCode);
        Serial.println("Make sure tx-server is running: cd tx-server && node server.mjs");
    }

    http.end();
    return txBytes;
}

// Fetch SUI balance from blockchain
String fetchSUIBalance() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected!");
        return "0";
    }

    HTTPClient http;
    http.begin(SUI_RPC_URL);
    http.addHeader("Content-Type", "application/json");

    // SUI RPC JSON-RPC request to get balance
    String payload = "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"suix_getBalance\",\"params\":[\"";
    payload += SUI_ADDRESS;
    payload += "\"]}";

    Serial.println("Fetching SUI balance...");
    int httpCode = http.POST(payload);

    String balance = "0";
    if (httpCode == 200) {
        String response = http.getString();
        Serial.println("Response: " + response);

        // Parse JSON response
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error) {
            if (doc.containsKey("result") && doc["result"].containsKey("totalBalance")) {
                long long totalBalance = doc["result"]["totalBalance"];
                // Convert from MIST to SUI (1 SUI = 1,000,000,000 MIST)
                float suiBalance = totalBalance / 1000000000.0;
                balance = String(suiBalance, 6);  // 6 decimal places
                Serial.println("Balance: " + balance + " SUI");
            }
        } else {
            Serial.println("JSON parse error!");
        }
    } else {
        Serial.print("HTTP error code: ");
        Serial.println(httpCode);
    }

    http.end();
    return balance;
}

// Initialize SUI client and keypair
void initializeSUI() {
    if (suiInitialized) {
        Serial.println("SUI already initialized");
        return;
    }

    Serial.println("Initializing SUI client and keypair...");

    // Create SUI client
    suiClient = SuiClient_newClient(SUI_RPC_URL);
    Serial.println("  SUI client created");

    // Load keypair from private key
    suiKeypair = SuiKeypair_fromSecretKey(SUI_PRIVATE_KEY);
    Serial.print("  Keypair loaded - Address: ");
    Serial.println(suiKeypair.toSuiAddress(&suiKeypair));

    suiInitialized = true;
    Serial.println("SUI initialization complete!");
}

// Initialize IMU sensor
void initializeIMU() {
    if (imuInitialized) {
        Serial.println("IMU already initialized");
        return;
    }

    Serial.println("Initializing QMI8658 IMU sensor...");

    if (QMI8658_init()) {
        imuInitialized = true;
        Serial.println("✓ IMU initialized successfully!");
        Serial.println("  Step counter ready");
    } else {
        Serial.println("✗ IMU initialization failed!");
    }
}

// Step detection algorithm with improved sensitivity
void detectStep() {
    if (!imuInitialized) {
        static unsigned long lastWarning = 0;
        if (millis() - lastWarning > 5000) {
            Serial.println("WARNING: IMU not initialized, step detection disabled");
            lastWarning = millis();
        }
        return;
    }

    float acc[3];
    float gyro[3];
    unsigned int timestamp;

    // Read accelerometer data
    QMI8658_read_xyz(acc, gyro, &timestamp);

    // Calculate vertical acceleration (Z-axis typically points up when device is flat)
    // For step detection, we mainly care about vertical motion
    float verticalAcc = abs(acc[2]);  // Z-axis acceleration

    // Calculate total acceleration magnitude for reference
    float accMagnitude = sqrt(acc[0]*acc[0] + acc[1]*acc[1] + acc[2]*acc[2]);

    // Debug logging every 2 seconds
    static unsigned long lastDebug = 0;
    if (millis() - lastDebug > 2000) {
        Serial.print("IMU Debug - Acc: [");
        Serial.print(acc[0], 2);
        Serial.print(", ");
        Serial.print(acc[1], 2);
        Serial.print(", ");
        Serial.print(acc[2], 2);
        Serial.print("] Mag: ");
        Serial.print(accMagnitude, 2);
        Serial.print(" Steps: ");
        Serial.println(stepCount);
        lastDebug = millis();
    }

    // Improved step detection:
    // Look for acceleration changes (peaks) that indicate steps
    // Using derivative approach: detect when acceleration increases then decreases

    unsigned long currentTime = millis();

    // Check if acceleration magnitude changed significantly
    float accChange = accMagnitude - lastAccMagnitude;

    // Detect step: rising edge (acceleration increasing above threshold)
    if (accChange > stepThreshold && !stepDetected) {
        // Check cooldown period
        if (currentTime - lastStepTime > stepCooldown) {
            stepDetected = true;
            stepCount++;
            lastStepTime = currentTime;

            Serial.print("✓ Step detected! Count: ");
            Serial.print(stepCount);
            Serial.print(" (AccChange: ");
            Serial.print(accChange, 2);
            Serial.println(")");
        }
    }

    // Reset step detection when acceleration stabilizes
    if (abs(accChange) < (stepThreshold * 0.3)) {
        stepDetected = false;
    }

    lastAccMagnitude = accMagnitude;
}

// Reset step counter
extern "C" void resetStepCounter() {
    stepCount = 0;
    Serial.println("Step counter reset to 0");
}

// Show progress screen (for real-time updates during signing)
void showProgressScreen(const char* title, const char* message) {
    Serial.println("Showing progress screen...");

    // Delete existing status screen if any
    if (statusScreen != NULL) {
        lv_obj_del(statusScreen);
        statusScreen = NULL;
    }

    // Create new status screen
    statusScreen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(statusScreen, lv_color_black(), 0);
    lv_obj_clear_flag(statusScreen, LV_OBJ_FLAG_SCROLLABLE);

    // Title label
    statusLabel = lv_label_create(statusScreen);
    lv_label_set_text(statusLabel, title);
    lv_obj_set_style_text_font(statusLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_x(statusLabel, 0);
    lv_obj_set_y(statusLabel, -60);
    lv_obj_set_align(statusLabel, LV_ALIGN_CENTER);

    // Progress spinner
    statusIcon = lv_spinner_create(statusScreen, 1000, 60);
    lv_obj_set_size(statusIcon, 80, 80);
    lv_obj_set_align(statusIcon, LV_ALIGN_CENTER);
    lv_obj_set_style_arc_color(statusIcon, lv_color_hex(0x00AAFF), LV_PART_INDICATOR);

    // Message label
    statusMessage = lv_label_create(statusScreen);
    lv_label_set_text(statusMessage, message);
    lv_obj_set_style_text_font(statusMessage, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(statusMessage, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(statusMessage, 200);
    lv_obj_set_x(statusMessage, 0);
    lv_obj_set_y(statusMessage, 60);
    lv_obj_set_align(statusMessage, LV_ALIGN_CENTER);
    lv_label_set_long_mode(statusMessage, LV_LABEL_LONG_WRAP);

    // Load progress screen
    lv_scr_load(statusScreen);
    lv_timer_handler(); // Update display immediately

    Serial.println("Progress screen shown");
}

// Update progress message
void updateProgressMessage(const char* message) {
    if (statusMessage != NULL) {
        lv_label_set_text(statusMessage, message);
        lv_timer_handler(); // Update display
    }
}

// Show status screen with result
void showStatusScreen(bool success, const char* title, const char* message, int autoCloseMs = 3000) {
    Serial.println("Showing status screen...");

    // Delete existing status screen if any
    if (statusScreen != NULL) {
        lv_obj_del(statusScreen);
        statusScreen = NULL;
    }

    // Create new status screen
    statusScreen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(statusScreen, lv_color_black(), 0);
    lv_obj_clear_flag(statusScreen, LV_OBJ_FLAG_SCROLLABLE);

    // Title label
    statusLabel = lv_label_create(statusScreen);
    lv_label_set_text(statusLabel, title);
    lv_obj_set_style_text_font(statusLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_x(statusLabel, 0);
    lv_obj_set_y(statusLabel, -60);
    lv_obj_set_align(statusLabel, LV_ALIGN_CENTER);

    // Status icon (checkmark or X)
    statusIcon = lv_label_create(statusScreen);
    if (success) {
        lv_label_set_text(statusIcon, LV_SYMBOL_OK);
        lv_obj_set_style_text_color(statusIcon, lv_color_hex(0x00FF00), 0); // Green
    } else {
        lv_label_set_text(statusIcon, LV_SYMBOL_CLOSE);
        lv_obj_set_style_text_color(statusIcon, lv_color_hex(0xFF0000), 0); // Red
    }
    lv_obj_set_style_text_font(statusIcon, &lv_font_montserrat_48, 0);
    lv_obj_set_align(statusIcon, LV_ALIGN_CENTER);

    // Message label
    statusMessage = lv_label_create(statusScreen);
    lv_label_set_text(statusMessage, message);
    lv_obj_set_style_text_font(statusMessage, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(statusMessage, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(statusMessage, 200);
    lv_obj_set_x(statusMessage, 0);
    lv_obj_set_y(statusMessage, 60);
    lv_obj_set_align(statusMessage, LV_ALIGN_CENTER);
    lv_label_set_long_mode(statusMessage, LV_LABEL_LONG_WRAP);

    // Load status screen
    lv_scr_load(statusScreen);
    lv_timer_handler(); // Update display immediately

    Serial.print("Status screen shown (auto-close in ");
    Serial.print(autoCloseMs);
    Serial.println("ms)");

    // Auto close and return to Screen1 after delay
    if (autoCloseMs > 0) {
        delay(autoCloseMs);
        _ui_screen_change(&ui_Screen1, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, &ui_Screen1_screen_init);
    }
}

// Test offline signing (sign without executing)
void testOfflineSign() {
    Serial.println("\n========================================");
    Serial.println("=== Testing Offline Sign ===");
    Serial.println("========================================");

    if (!suiInitialized) {
        Serial.println("ERROR: SUI not initialized!");
        return;
    }

    Serial.println("\n[1/2] Loading prebuilt transaction bytes...");
    Serial.print("Transaction bytes length: ");
    Serial.println(strlen(TRANSACTION_BYTES));
    Serial.println("DONE!");

    Serial.println("\n[2/2] Generating signature from transaction...");

    // Sign the transaction to get signature (offline, no network needed)
    SuiSignature sig = suiKeypair.signTransaction(&suiKeypair, TRANSACTION_BYTES);

    Serial.println("DONE! Signature created!");

    // Display results
    Serial.println("\n--- Offline Sign Result ---");

    if (sig.signature) {
        Serial.println("  ✓ Signature in BASE64 format:");
        Serial.print("    ");
        Serial.println(sig.signature);
    }

    if (sig.bytes) {
        char sui_sig_hex[195]; // 2 hex chars per byte + null terminator
        bytes_to_hex(sig.bytes, 97, sui_sig_hex); // 97 bytes is the length of a Sui Signature

        Serial.println("  ✓ Signature in HEX format:");
        Serial.print("    ");
        Serial.println(sui_sig_hex);
    }

    Serial.println("\n✓ Offline signing test completed!");
    Serial.println("  You can use this signature to execute the transaction later");
    Serial.println("  without needing the private key online.");
    Serial.println("========================================\n");
}

// FreeRTOS task for offline signing (no execution)
// This runs in a separate task to avoid blocking UI
void transactionTask(void *parameter) {
    Serial.println("\n=== Offline Sign Task Started ===");
    Serial.print("Running on core: ");
    Serial.println(xPortGetCoreID());

    // Show progress screen immediately
    showProgressScreen("Signing...", "Loading transaction");

    if (!suiInitialized) {
        Serial.println("ERROR: SUI not initialized!");
        showStatusScreen(false, "Error", "SUI not initialized", 3000);
        transactionInProgress = false;
        vTaskDelete(NULL);
        return;
    }

    Serial.println("\n=== Offline Sign Transaction (No Network Needed) ===");
    Serial.println("This will generate a signature without executing the transaction");

    // Load prebuilt transaction bytes
    Serial.println("\n[1/2] Loading transaction bytes...");
    updateProgressMessage("Loading tx bytes...");
    Serial.print("Transaction bytes length: ");
    Serial.println(strlen(TRANSACTION_BYTES));
    Serial.println("DONE!");

    // Sign the transaction offline (no WiFi/network needed)
    Serial.println("\n[2/2] Generating signature...");
    updateProgressMessage("Generating\nsignature...");
    Serial.println("(This is done locally, no network connection required)");

    SuiSignature sig = suiKeypair.signTransaction(&suiKeypair, TRANSACTION_BYTES);

    Serial.println("DONE! Signature created!");

    // Display results
    Serial.println("\n========================================");
    Serial.println("--- Offline Sign Result ---");
    Serial.println("========================================");

    if (sig.signature) {
        Serial.println("\n✓ Signature in BASE64 format:");
        Serial.print("  ");
        Serial.println(sig.signature);
        Serial.println("\n  This signature can be used to execute the transaction later");
        Serial.println("  on any device without needing the private key!");
    } else {
        Serial.println("\n✗ Failed to generate signature!");
    }

    if (sig.bytes) {
        char sui_sig_hex[195]; // 2 hex chars per byte + null terminator
        bytes_to_hex(sig.bytes, 97, sui_sig_hex); // 97 bytes is the length of a Sui Signature

        Serial.println("\n✓ Signature in HEX format:");
        Serial.print("  ");
        Serial.println(sui_sig_hex);
    }

    Serial.println("\n========================================");
    Serial.println("✓ Offline signing completed successfully!");
    Serial.println("  - Transaction was NOT executed (no network call)");
    Serial.println("  - Signature is ready to be broadcast");
    Serial.println("  - You can execute this later with the signature");
    Serial.println("========================================\n");

    // Show status screen
    bool success = (sig.signature != NULL);
    if (success) {
        showStatusScreen(true, "Offline Sign", "Signature created!\nReady to broadcast", 3000);
    } else {
        showStatusScreen(false, "Sign Failed", "Could not generate\nsignature", 3000);
    }

    // Clear flag and delete task
    transactionInProgress = false;
    Serial.println("Offline sign task complete");

    vTaskDelete(NULL);
}

// FreeRTOS task for Sign THEN Execute (2-step process)
void signAndExecuteTask(void *parameter) {
    Serial.println("\n=== Sign and Execute Task Started (2-Step Process) ===");
    Serial.print("Running on core: ");
    Serial.println(xPortGetCoreID());

    // Show progress screen immediately
    showProgressScreen("Processing...", "Checking WiFi");

    // Check WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected! Attempting reconnection...");
        updateProgressMessage("Reconnecting WiFi...");
        connectWiFi();

        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Failed to reconnect WiFi!");
            showStatusScreen(false, "Error", "WiFi not connected", 3000);
            transactionInProgress = false;
            vTaskDelete(NULL);
            return;
        }
    }

    if (!suiInitialized) {
        Serial.println("ERROR: SUI not initialized!");
        showStatusScreen(false, "Error", "SUI not initialized", 3000);
        transactionInProgress = false;
        vTaskDelete(NULL);
        return;
    }

    Serial.println("\n=== Step 0: Fetch Fresh Transaction from Server ===");
    Serial.print("Free heap: ");
    Serial.println(ESP.getFreeHeap());

    // Fetch fresh transaction bytes from server
    Serial.println("\n[1/5] Fetching transaction from server...");
    updateProgressMessage("Fetching from\nserver...");

    String freshTxBytes = fetchTransactionFromServer(1000000); // 0.001 SUI

    if (freshTxBytes.length() == 0) {
        Serial.println("ERROR: Failed to fetch transaction from server!");
        Serial.println("Falling back to hardcoded TRANSACTION_BYTES...");
        updateProgressMessage("Using fallback\ntransaction...");
        freshTxBytes = String(TRANSACTION_BYTES);
    }

    Serial.println("\n=== Step 1: Sign Transaction (Offline) ===");
    Serial.println("Generating signature locally without network...");

    // Load transaction bytes (from server or fallback)
    Serial.println("\n[2/5] Loading transaction bytes...");
    updateProgressMessage("Loading tx bytes...");
    MicroSuiTransaction tx = SuiTransaction_setPrebuiltTxBytes(freshTxBytes.c_str());
    Serial.println("Transaction loaded");

    // Sign offline (no WiFi needed for this step)
    Serial.println("\n[3/5] Signing transaction offline...");
    updateProgressMessage("Signing\ntransaction...");
    Serial.println("(This is done locally, no network required)");

    // Convert tx bytes to hex for signing
    char* tx_bytes_hex = (char*)malloc(tx.tx_bytes.length * 2 + 1);
    if (!tx_bytes_hex) {
        Serial.println("ERROR: Failed to allocate memory for tx bytes!");
        showStatusScreen(false, "Error", "Out of memory", 3000);
        tx.clear(&tx);
        transactionInProgress = false;
        vTaskDelete(NULL);
        return;
    }
    bytes_to_hex(tx.tx_bytes.data, tx.tx_bytes.length, tx_bytes_hex);

    // Generate signature
    SuiSignature sig = suiKeypair.signTransaction(&suiKeypair, tx_bytes_hex);
    free(tx_bytes_hex);

    Serial.println("✓ Signature generated successfully!");
    if (sig.signature) {
        Serial.print("  Signature (BASE64): ");
        Serial.println(sig.signature);
    }

    // Now execute with the signature
    Serial.println("\n=== Step 2: Execute Transaction (Online) ===");
    Serial.println("Sending signed transaction to blockchain...");

    // Verify WiFi is connected for execution
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("ERROR: WiFi not connected! Cannot execute online.");
        showStatusScreen(false, "Error", "WiFi disconnected", 3000);
        tx.clear(&tx);
        transactionInProgress = false;
        vTaskDelete(NULL);
        return;
    }

    // Stabilize WiFi before network call
    Serial.println("\n[4/5] Preparing for network transmission...");
    updateProgressMessage("Preparing\nnetwork...");
    Serial.print("WiFi Status: ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
    Serial.print("WiFi RSSI: ");
    Serial.println(WiFi.RSSI());

    // Ensure WiFi power save is disabled
    WiFi.setSleep(false);

    // Small delay for WiFi stability
    for(int i = 0; i < 3; i++) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        Serial.print(".");
    }
    Serial.println(" Ready!");

    // Execute the signed transaction
    Serial.println("\n[5/5] Executing signed transaction...");
    updateProgressMessage("Sending to\nblockchain...");
    Serial.println("(Sending to SUI network with signature)");
    Serial.println(">>> Calling executeTransactionBlock NOW <<<");
    unsigned long startTime = millis();

    // Call the separate execute function with signature
    SuiTransactionBlockResponse res = suiClient.executeTransactionBlock(&suiClient, tx.tx_bytes, sig);

    unsigned long endTime = millis();
    Serial.print(">>> Call completed in ");
    Serial.print(endTime - startTime);
    Serial.println(" ms <<<");

    // Give time for response processing
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Check results
    Serial.println("\n=== Processing Response ===");
    updateProgressMessage("Processing\nresponse...");
    Serial.print("WiFi Status after: ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");

    Serial.println("\n========================================");
    Serial.println("--- Transaction Result ---");
    Serial.println("========================================");

    // Debug: Check what we got
    Serial.println("\nDEBUG Response:");
    Serial.print("  res.digest: ");
    Serial.println(res.digest ? res.digest : "NULL");
    Serial.print("  res.checkpoint: ");
    Serial.println(res.checkpoint ? res.checkpoint : "NULL");
    Serial.print("  res.confirmedLocalExecution: ");
    Serial.println(res.confirmedLocalExecution ? res.confirmedLocalExecution : "NULL");
    Serial.print("  res.balanceChanges_len: ");
    Serial.println(res.balanceChanges_len);

    bool success = false;

    if (res.digest && strcmp(res.digest, "[parse_error]") != 0 && strlen(res.digest) > 0) {
        Serial.println("\n✓ Transaction executed successfully!");
        Serial.print("  Digest (Tx ID): ");
        Serial.println(res.digest);
        success = true;
    } else {
        Serial.println("\n✗ Transaction failed or parse error!");
        Serial.println("  Possible causes:");
        Serial.println("    - Invalid transaction bytes");
        Serial.println("    - Transaction already executed");
        Serial.println("    - Insufficient gas/balance");
        Serial.println("    - Network response parsing error");
    }

    if (res.checkpoint) {
        Serial.print("  Checkpoint: ");
        Serial.println(res.checkpoint);
    }
    if (res.confirmedLocalExecution) {
        Serial.print("  Confirmed: ");
        Serial.println(res.confirmedLocalExecution);
    }

    Serial.print("\n  Balance Changes: ");
    Serial.println(res.balanceChanges_len);

    for (int i = 0; i < res.balanceChanges_len; i++) {
        Serial.print("    #");
        Serial.println(i + 1);
        if (res.balanceChanges[i].amount) {
            Serial.print("      amount: ");
            Serial.println(res.balanceChanges[i].amount);
        }
        if (res.balanceChanges[i].coinType) {
            Serial.print("      coinType: ");
            Serial.println(res.balanceChanges[i].coinType);
        }
    }

    // Clean up transaction
    tx.clear(&tx);

    if (success) {
        Serial.println("\n========================================");
        Serial.println("✓ Transaction completed successfully!");
        Serial.println("  - Transaction was executed on blockchain");
        Serial.println("  - Changes are now permanent");
        Serial.println("========================================\n");

        // Show success status screen
        String txId = String(res.digest);
        String successMsg = "Tx: " + txId.substring(0, 8) + "...\nExecuted on chain!";
        showStatusScreen(true, "Success!", successMsg.c_str(), 3000);

        // Update balance after successful transaction
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Updating balance...");
            current_balance = fetchSUIBalance();
            if (ui_balance != NULL) {
                lv_label_set_text(ui_balance, current_balance.c_str());
                Serial.println("Balance updated on UI");
            }
        }
    } else {
        Serial.println("\n========================================");
        Serial.println("✗ Transaction failed!");
        Serial.println("  Please check your WiFi and try again");
        Serial.println("========================================\n");

        // Show failure status screen
        showStatusScreen(false, "Failed!", "Transaction failed\nPlease try again", 3000);
    }

    // Clear flag and delete task
    transactionInProgress = false;
    Serial.println("Sign and execute task complete");

    vTaskDelete(NULL);
}

// C wrapper for offline signing (Button1 in Screen1)
extern "C" void executeOfflineSign() {
    // Don't start a new signing if one is already running
    if (transactionInProgress) {
        Serial.println("Offline signing already in progress, please wait...");
        return;
    }

    // Set flag to indicate signing is starting
    transactionInProgress = true;

    Serial.println("\n========================================");
    Serial.println("Button 1 clicked - starting OFFLINE SIGNING...");
    Serial.println("This will NOT execute on network, only generate signature");
    Serial.println("========================================");

    // Create FreeRTOS task for offline signing
    // Priority 1 = lower priority task
    // Stack size 8192 bytes
    // Pinned to core 1 (separate from WiFi on core 0)
    BaseType_t taskCreated = xTaskCreatePinnedToCore(
        transactionTask,           // Task function (offline sign)
        "OfflineSignTask",         // Task name
        8192,                      // Stack size
        NULL,                      // Parameters
        1,                         // Priority
        &transactionTaskHandle,    // Task handle
        1                          // Core 1 (separate from WiFi)
    );

    if (taskCreated != pdPASS) {
        Serial.println("ERROR: Failed to create offline sign task!");
        transactionInProgress = false;
    } else {
        Serial.println("Offline sign task created successfully");
        Serial.println("Check Serial Monitor for signature output");
    }
}

// C wrapper for sign and execute (Button2 in Screen3)
extern "C" void executeSignAndExecute() {
    // Don't start a new transaction if one is already running
    if (transactionInProgress) {
        Serial.println("Transaction already in progress, please wait...");
        return;
    }

    // Check WiFi before starting
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("ERROR: WiFi not connected! Cannot execute transaction online.");
        Serial.println("Please check your WiFi connection and try again.");
        return;
    }

    // Set flag to indicate transaction is starting
    transactionInProgress = true;

    Serial.println("\n========================================");
    Serial.println("Button 2 clicked - SIGN + EXECUTE (2-Step)");
    Serial.println("Step 1: Sign offline (generate signature)");
    Serial.println("Step 2: Execute online (send to blockchain)");
    Serial.println("========================================");

    // Create FreeRTOS task for sign and execute
    // IMPORTANT: Run on same core as WiFi (core 0) for better WiFi stability
    // Increased stack size to 16KB to handle HTTP requests
    // Priority 2 = still lower than WiFi task but higher than other tasks
    BaseType_t taskCreated = xTaskCreatePinnedToCore(
        signAndExecuteTask,        // Task function (sign & execute)
        "SignExecuteTask",         // Task name
        16384,                     // Stack size (16KB - increased from 8KB)
        NULL,                      // Parameters
        2,                         // Priority (slightly higher for WiFi operations)
        &transactionTaskHandle,    // Task handle
        0                          // Core 0 (SAME as WiFi for better stability)
    );

    if (taskCreated != pdPASS) {
        Serial.println("ERROR: Failed to create sign & execute task!");
        transactionInProgress = false;
    } else {
        Serial.println("Sign & execute task created successfully");
        Serial.println("Running on same core as WiFi for stability");
        Serial.println("Check Serial Monitor for transaction result");
    }
}

void setup()
{
    Serial.begin( 115200 ); /* prepare for possible serial debug */
    delay(1000);

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino with Custom LCD Driver" );

    // Initialize PSRAM
    Serial.println("Initializing PSRAM...");
    if(psramInit()){
        Serial.println("PSRAM is correctly initialized");
    }else{
        Serial.println("PSRAM not available");
    }

    // Allocate frame buffer in PSRAM (240*240*2 = 115200 bytes)
    UDOUBLE Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
    if ((BlackImage = (UWORD *)ps_malloc(Imagesize)) == NULL){
        Serial.println("Failed to allocate BlackImage in PSRAM!");
        while(1);
    }
    Serial.print("BlackImage allocated in PSRAM: ");
    Serial.print(Imagesize);
    Serial.println(" bytes");

    // Initialize GPIO and LCD
    Serial.println("Initializing LCD hardware...");
    if (DEV_Module_Init() != 0){
        Serial.println("GPIO Init Fail!");
        while(1);
    }
    Serial.println("GPIO Init successful!");

    // Initialize LCD
    LCD_1IN28_Init(HORIZONTAL);
    Serial.println("LCD initialized");

    // Clear LCD to black
    LCD_1IN28_Clear(0x0000);  // BLACK

    Serial.println("Initializing LVGL...");
    lv_init();
#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif
    Serial.println("LVGL initialized");

    // Initialize touch controller
    Serial.println("Initializing touch controller...");
    touch.begin();
    Serial.println("Touch initialized");

    // Initialize IMU sensor (QMI8658)
    Serial.println("Initializing IMU sensor...");
    initializeIMU();

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register( &indev_drv );

    // Setup LVGL timer
    const esp_timer_create_args_t lvgl_tick_timer_args = {
      .callback = &example_increase_lvgl_tick,
      .name = "lvgl_tick"
    };

    esp_timer_handle_t lvgl_tick_timer = NULL;
    esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
    esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000);

    // Show splash screen with "COMMAND OSS" - animated colors
    Serial.println("Showing splash screen...");
    lv_obj_t * splash_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(splash_screen, lv_color_black(), 0);

    lv_obj_t * splash_label = lv_label_create(splash_screen);
    lv_label_set_text(splash_label, "COMMAND OSS");
    lv_obj_set_style_text_font(splash_label, &lv_font_montserrat_24, 0);
    lv_obj_center(splash_label);

    lv_scr_load(splash_screen);

    // Animate colors - change every 0.5 seconds
    lv_color_t colors[] = {
        lv_color_hex(0xFF0000),  // Red
        lv_color_hex(0x00FF00),  // Green
        lv_color_hex(0x0000FF),  // Blue
        lv_color_hex(0xFFFF00),  // Yellow
        lv_color_hex(0xFF00FF),  // Magenta
        lv_color_hex(0x00FFFF),  // Cyan
        lv_color_hex(0xFFFFFF)   // White
    };
    int num_colors = sizeof(colors) / sizeof(colors[0]);

    // Show splash for 3.5 seconds with color animation
    for(int color_idx = 0; color_idx < num_colors; color_idx++) {
        lv_obj_set_style_text_color(splash_label, colors[color_idx], 0);

        // Update screen for 0.5 seconds
        for(int i = 0; i < 17; i++) {  // 17 * 30ms ≈ 500ms
            lv_timer_handler();
            delay(30);
        }
    }

    Serial.println("Splash screen shown");

    // Connect to WiFi
    connectWiFi();

    // Initialize SUI client and keypair
    if (WiFi.status() == WL_CONNECTED) {
        initializeSUI();

        // Test offline signing feature
        testOfflineSign();
    }

    // Load SquareLine Studio UI
    Serial.println("Loading SquareLine Studio UI...");
    ui_init();
    Serial.println("UI loaded!");

    // Set address display with shortened SUI address
    String shortAddr = shortenAddress(SUI_ADDRESS);
    lv_label_set_text(ui_address, shortAddr.c_str());

    // Fetch initial balance
    if (WiFi.status() == WL_CONNECTED) {
        current_balance = fetchSUIBalance();
        lv_label_set_text(ui_balance, current_balance.c_str());
    } else {
        lv_label_set_text(ui_balance, "No WiFi");
    }

    Serial.println( "Setup done" );
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */

    // If transaction is in progress, skip non-critical operations
    // to reduce WiFi interference
    if (transactionInProgress) {
        delay(10);  // Longer delay during transaction to give WiFi priority
        yield();    // Give WiFi stack time to process
        return;
    }

    unsigned long current_time = millis();

    // Update countdown timer for Screen3
    if (current_time - last_countdown_update >= countdown_interval) {
        last_countdown_update = current_time;

        // Decrement countdown
        countdown_value--;
        if (countdown_value < 0) {
            countdown_value = 60; // Reset to 60
        }

        // Update ui_time1 label if it exists
        if (ui_time1 != NULL) {
            char time_str[4];
            snprintf(time_str, sizeof(time_str), "%d", countdown_value);
            lv_label_set_text(ui_time1, time_str);
        }

        // Update Arc1 value to match countdown if it exists
        if (ui_Arc1 != NULL) {
            lv_arc_set_value(ui_Arc1, countdown_value);
        }
    }

    // Update balance periodically (every 30 seconds)
    if (current_time - last_balance_update >= balance_update_interval) {
        last_balance_update = current_time;

        // Reconnect WiFi if disconnected
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi disconnected, reconnecting...");
            connectWiFi();
        }

        // Fetch and update balance
        if (WiFi.status() == WL_CONNECTED) {
            current_balance = fetchSUIBalance();
            if (ui_balance != NULL) {
                lv_label_set_text(ui_balance, current_balance.c_str());
            }
        }
    }

    // Step detection (run frequently for accuracy)
    static unsigned long last_step_check = 0;
    if (current_time - last_step_check >= 50) {  // Check every 50ms
        last_step_check = current_time;
        detectStep();

        // Update Screen5 UI if active
        if (ui_LabelStepCount != NULL && ui_Arc_Steps != NULL) {
            char stepStr[10];
            snprintf(stepStr, sizeof(stepStr), "%d", stepCount);
            lv_label_set_text(ui_LabelStepCount, stepStr);

            // Update arc (max 1000 steps display, loop after)
            int arcValue = stepCount % 1000;
            if (arcValue == 0 && stepCount > 0) {
                arcValue = 1000;  // Show full when multiple of 1000
            }
            lv_arc_set_value(ui_Arc_Steps, arcValue);
        }
    }

    delay( 5 );
}
