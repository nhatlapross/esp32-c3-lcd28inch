#!/usr/bin/env node
/**
 * Auto-generate fresh transaction bytes for ESP32
 * Run: node generate_tx.mjs
 */

import { Transaction } from '@mysten/sui/transactions';
import { getFullnodeUrl, SuiClient } from '@mysten/sui/client';
import { readFileSync, writeFileSync } from 'fs';

const SENDER_ADDRESS = '0xb0bd39aa7e029a1c61584fded4c78529eced33d3e43ef54adf50b1e151efefe3';
const INO_FILE = './src/ESP32S3_Squareline_UI/ESP32S3_Squareline_UI.ino';

async function generateAndUpdateTransaction() {
    console.log('🔄 Generating fresh transaction bytes...\n');

    const client = new SuiClient({ url: getFullnodeUrl('testnet') });

    try {
        // Create transaction
        const tx = new Transaction();
        tx.setSender(SENDER_ADDRESS);

        // Simple test: Split 0.001 SUI and transfer to self
        const [coin] = tx.splitCoins(tx.gas, [1_000_000]);
        tx.transferObjects([coin], SENDER_ADDRESS);

        console.log('📦 Building transaction...');
        const txBytes = await tx.build({ client });
        const txHex = Buffer.from(txBytes).toString('hex');

        console.log('✓ Transaction bytes created!\n');
        console.log('Transaction details:');
        console.log('  Sender:', SENDER_ADDRESS);
        console.log('  Type: Split 0.001 SUI and transfer to self');
        console.log('  Bytes length:', txHex.length / 2, 'bytes');
        console.log('  Network: Testnet\n');

        // Read current .ino file
        console.log('📝 Updating .ino file...');
        let content = readFileSync(INO_FILE, 'utf8');

        // Find and replace TRANSACTION_BYTES
        const regex = /const char\* TRANSACTION_BYTES = "([0-9a-f]+)";/;
        const match = content.match(regex);

        if (match) {
            const oldBytes = match[1];
            content = content.replace(oldBytes, txHex);
            writeFileSync(INO_FILE, content, 'utf8');

            console.log('✓ Updated TRANSACTION_BYTES in', INO_FILE);
            console.log('  Old bytes:', oldBytes.substring(0, 32) + '...');
            console.log('  New bytes:', txHex.substring(0, 32) + '...\n');

            console.log('✅ Done! Upload the sketch to ESP32 to test.\n');
            console.log('💡 Tip: Run this script before each test to get fresh transaction bytes!');
        } else {
            console.error('❌ Could not find TRANSACTION_BYTES in .ino file');
            console.log('\nManually copy this value:');
            console.log('========================================');
            console.log(txHex);
            console.log('========================================');
        }

    } catch (error) {
        console.error('❌ Error:', error.message);
        process.exit(1);
    }
}

generateAndUpdateTransaction();
