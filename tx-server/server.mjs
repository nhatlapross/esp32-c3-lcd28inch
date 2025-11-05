#!/usr/bin/env node
/**
 * Simple Transaction Builder Server for ESP32
 * Run: node server.mjs
 * ESP32 will call: GET /build-transaction to get fresh transaction bytes
 */

import { Transaction } from '@mysten/sui/transactions';
import { getFullnodeUrl, SuiClient } from '@mysten/sui/client';
import express from 'express';
import cors from 'cors';

const app = express();
const PORT = 3000;

// Configuration
const SENDER_ADDRESS = '0xb0bd39aa7e029a1c61584fded4c78529eced33d3e43ef54adf50b1e151efefe3';
const NETWORK = 'testnet'; // or 'mainnet', 'devnet'

// Initialize Sui client
const client = new SuiClient({ url: getFullnodeUrl(NETWORK) });

// Middleware
app.use(cors());
app.use(express.json());

// Health check endpoint
app.get('/', (req, res) => {
    res.json({
        status: 'ok',
        service: 'Sui Transaction Builder for ESP32',
        network: NETWORK,
        sender: SENDER_ADDRESS
    });
});

// Build transaction endpoint
app.get('/build-transaction', async (req, res) => {
    try {
        console.log('\n📦 Building new transaction...');

        // Get optional parameters from query
        const amount = parseInt(req.query.amount) || 1_000_000; // Default: 0.001 SUI
        const recipient = req.query.recipient || SENDER_ADDRESS; // Default: self

        console.log('  Amount:', amount, 'MIST');
        console.log('  Recipient:', recipient);

        // Create transaction
        const tx = new Transaction();
        tx.setSender(SENDER_ADDRESS);

        // Split coins and transfer
        const [coin] = tx.splitCoins(tx.gas, [amount]);
        tx.transferObjects([coin], recipient);

        // Build transaction bytes
        const txBytes = await tx.build({ client });
        const txHex = Buffer.from(txBytes).toString('hex');

        console.log('✓ Transaction built successfully!');
        console.log('  Bytes length:', txHex.length / 2, 'bytes');

        // Return transaction bytes as JSON
        res.json({
            success: true,
            txBytes: txHex,
            details: {
                sender: SENDER_ADDRESS,
                recipient: recipient,
                amount: amount,
                amountSUI: (amount / 1_000_000_000).toFixed(9),
                network: NETWORK,
                timestamp: new Date().toISOString()
            }
        });

    } catch (error) {
        console.error('❌ Error building transaction:', error.message);
        res.status(500).json({
            success: false,
            error: error.message
        });
    }
});

// Custom transaction endpoint (advanced)
app.post('/build-custom-transaction', async (req, res) => {
    try {
        const { recipient, amount, memo } = req.body;

        if (!recipient || !amount) {
            return res.status(400).json({
                success: false,
                error: 'Missing required fields: recipient, amount'
            });
        }

        console.log('\n📦 Building custom transaction...');
        console.log('  Recipient:', recipient);
        console.log('  Amount:', amount, 'MIST');

        const tx = new Transaction();
        tx.setSender(SENDER_ADDRESS);

        const [coin] = tx.splitCoins(tx.gas, [amount]);
        tx.transferObjects([coin], recipient);

        const txBytes = await tx.build({ client });
        const txHex = Buffer.from(txBytes).toString('hex');

        console.log('✓ Custom transaction built!');

        res.json({
            success: true,
            txBytes: txHex,
            details: {
                sender: SENDER_ADDRESS,
                recipient: recipient,
                amount: amount,
                amountSUI: (amount / 1_000_000_000).toFixed(9),
                network: NETWORK,
                memo: memo,
                timestamp: new Date().toISOString()
            }
        });

    } catch (error) {
        console.error('❌ Error:', error.message);
        res.status(500).json({
            success: false,
            error: error.message
        });
    }
});

// Start server
app.listen(PORT, '0.0.0.0', () => {
    console.log('');
    console.log('═══════════════════════════════════════════════════');
    console.log('🚀 Sui Transaction Builder Server');
    console.log('═══════════════════════════════════════════════════');
    console.log(`  Running on: http://localhost:${PORT}`);
    console.log(`  Network: ${NETWORK}`);
    console.log(`  Sender: ${SENDER_ADDRESS}`);
    console.log('');
    console.log('Endpoints:');
    console.log(`  GET  /build-transaction?amount=1000000&recipient=0x...`);
    console.log(`  POST /build-custom-transaction`);
    console.log('');
    console.log('💡 ESP32 will call this server to get fresh transaction bytes');
    console.log('═══════════════════════════════════════════════════');
    console.log('');
});
