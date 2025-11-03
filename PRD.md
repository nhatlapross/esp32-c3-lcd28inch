# Product Requirements Document (PRD)
# Blockchain-Powered Smart Lock System for Office Rentals

**Version:** 1.0
**Date:** November 1, 2025
**Status:** MVP Planning
**Project Codename:** SuiLock

---

## 1. Executive Summary

### 1.1 Overview
A blockchain-powered smart lock system enabling peer-to-peer office space rentals with trustless payments, automated access control, and per-minute billing. The system combines IoT hardware (ESP32 microcontroller with LCD display) with Sui blockchain smart contracts to create a decentralized, automated rental platform.

### 1.2 Vision
Build a flexible, blockchain-controlled physical access infrastructure that can serve multiple verticals (offices, parking, storage) with minimal friction for users and maximum security for transactions.

### 1.3 Mission (MVP)
Deliver a working prototype demonstrating:
- ESP32-based smart lock with 1.28" LCD display
- Sui blockchain integration via MicroSui library
- Tunnel-based payment channels for trustless escrow
- Web application for booking and payment
- 3D-printed mini office model for demonstration

---

## 2. Problem Statement

### 2.1 Current Pain Points

#### For Office Space Renters:
- **Inflexible rental periods**: Most platforms require full-day or monthly commitments
- **High platform fees**: Centralized platforms (WeWork, Regus) charge 20-30% commission
- **Trust issues**: Upfront payment with no guarantee of service quality
- **Manual processes**: Check-in/check-out requires human interaction
- **Limited micro-rental options**: Hard to rent for 1-2 hours

#### For Office Space Owners:
- **Payment delays**: Traditional platforms hold payments for 30+ days
- **High commission fees**: 20-30% platform cut
- **Limited control**: Can't set dynamic pricing or custom terms
- **Trust issues**: Risk of non-payment or property damage
- **Manual access management**: Keys, codes, physical check-ins

#### For Property Managers:
- **Access control complexity**: Managing keys/codes for multiple renters
- **No automated billing**: Manual time tracking and invoicing
- **Dispute resolution overhead**: Handling payment/access disputes manually

### 2.2 Why Existing Solutions Fall Short
- **WeWork/Regus**: High fees, inflexible terms, centralized control
- **LiquidSpace**: Still requires platform intermediary, high fees
- **Traditional Airbnb for offices**: No automated access control, trust issues
- **Smart lock solutions (August, Yale)**: No blockchain integration, centralized servers

---

## 3. Solution Overview

### 3.1 Core Innovation
**Tunnel + MicroSui Architecture**: Peer-to-peer payment channels with offline signature generation enable trustless escrow without continuous blockchain transactions.

### 3.2 How It Works

#### High-Level Flow:
```
1. Renter books office via web app
2. Smart contract creates Tunnel with deposits from both parties
3. ESP32 verifies payment and grants access
4. Lock opens, renter uses space
5. Time tracking happens on-device with cryptographic signatures
6. Renter leaves, ESP32 signs exit timestamp
7. Both parties settle via Tunnel (automatic fund distribution)
8. Lock secures space for next renter
```

#### Technical Architecture:
```
┌─────────────┐         ┌──────────────┐         ┌─────────────┐
│   Web App   │◄────────┤ Sui Blockchain├────────►│   ESP32     │
│  (NextJS)   │         │   (Tunnel)    │         │  + Lock     │
└─────────────┘         └──────────────┘         └─────────────┘
      │                                                  │
      │                                                  │
      ▼                                                  ▼
┌─────────────┐                                  ┌─────────────┐
│   Enoki     │                                  │  MicroSui   │
│   (Auth)    │                                  │  (Signing)  │
└─────────────┘                                  └─────────────┘
```

### 3.3 Key Components

#### Hardware:
- **ESP32-S3 or ESP32-C3**: Microcontroller with WiFi
- **1.28" Round LCD Display**: Shows status, QR codes, timers
- **CST816S Touch Controller**: Touch input capability
- **QMI8658 IMU**: Motion detection (optional anti-tamper)
- **Electronic Lock Module**: Solenoid/servo/electric strike
- **Power Supply**: 5V/12V depending on lock type

#### Software - Blockchain:
- **Tunnel Smart Contract**: P2P payment channel with escrow
- **MicroSui Library**: ESP32 signing capability (ED25519/BLS12381)
- **Sui SDK**: Blockchain interaction

#### Software - Web App:
- **NextJS + TypeScript**: Frontend and API
- **Sui Enoki**: Wallet abstraction and authentication
- **Vercel KV**: User data storage
- **React**: UI components

---

## 4. User Personas & Roles

### 4.1 Primary Users

#### Persona 1: The Freelancer (Renter)
- **Name**: Alex, 28, Freelance Designer
- **Needs**: Quiet workspace for client calls, 2-3 hours at a time
- **Pain Points**: Coffee shops are noisy, full-day rentals are expensive
- **Tech Savvy**: Medium (comfortable with apps, new to crypto)
- **Motivation**: Affordable, flexible workspace without long commitments

#### Persona 2: The Property Owner (Host)
- **Name**: Jordan, 45, Small Business Owner
- **Needs**: Monetize unused office space during off-hours
- **Pain Points**: Traditional platforms take 30% fees, manual key management
- **Tech Savvy**: Low-Medium (can use apps, hesitant about crypto)
- **Motivation**: Passive income, automated management

#### Persona 3: The Admin/Platform Operator (Optional for MVP)
- **Name**: Sam, 35, Platform Manager
- **Needs**: Oversee transactions, resolve disputes, manage hardware
- **Pain Points**: Scaling manual dispute resolution
- **Tech Savvy**: High (developer-level understanding)
- **Motivation**: Platform growth, fee revenue

### 4.2 User Roles & Permissions

| Role | Permissions | Actions |
|------|-------------|---------|
| **Renter** | Book spaces, make payments, access during rental period | Browse listings, create Tunnel deposit, unlock door, end rental |
| **Owner** | List spaces, set pricing, withdraw earnings | Create listings, configure lock hardware, respond to bookings, collect fees |
| **Admin** | Full platform access, dispute resolution | Manage users, resolve conflicts, view analytics, configure smart contracts |
| **Guest (Anonymous)** | Browse public listings | View available spaces and pricing (no booking) |

---

## 5. Core Features & Requirements

### 5.1 MVP Features (Must-Have)

#### 5.1.1 Smart Lock Hardware
| Feature | Description | Priority |
|---------|-------------|----------|
| **Unlock via Blockchain Verification** | ESP32 verifies Tunnel payment before unlocking | P0 |
| **LCD Status Display** | Show "Available", "In Use", "Locked", remaining time | P0 |
| **QR Code Display** | Display booking QR code for renter verification | P1 |
| **Time Tracking** | Log entry/exit timestamps with cryptographic signatures | P0 |
| **Offline Signing** | ESP32 signs access logs without internet using MicroSui | P0 |
| **Touch Input** | Allow manual unlock confirmation on device | P2 |
| **Anti-Tamper Detection** | Alert if device is moved/tampered (via IMU) | P2 |

#### 5.1.2 Blockchain Smart Contracts
| Feature | Description | Priority |
|---------|-------------|----------|
| **Tunnel Creation** | Create P2P payment channel with deposits from both parties | P0 |
| **Deposit Escrow** | Hold funds in smart contract until settlement | P0 |
| **Signature Verification** | Verify ESP32 and renter signatures on-chain | P0 |
| **Time-based Settlement** | Calculate payment based on actual usage time | P0 |
| **Penalty Mechanism** | Slash deposit if party doesn't cooperate in settlement | P1 |
| **Refund Logic** | Return unused deposit to renter automatically | P0 |
| **Multi-token Support** | Accept SUI and stablecoins (USDC) | P2 |

#### 5.1.3 Web Application
| Feature | Description | Priority |
|---------|-------------|----------|
| **User Authentication** | Login via Enoki (email/social) or wallet connect | P0 |
| **Browse Listings** | View available office spaces with photos, pricing, location | P0 |
| **Booking Flow** | Select time slot, create Tunnel deposit, confirm booking | P0 |
| **Payment Processing** | Fiat-to-SUI conversion for Web2 users | P1 |
| **Unlock Interface** | QR code or NFC tap to unlock door | P0 |
| **Active Rental Dashboard** | Show remaining time, extend rental option, end rental button | P0 |
| **Transaction History** | View past rentals and payments | P1 |
| **Wallet Management** | View SUI balance, add funds | P1 |

#### 5.1.4 Owner Portal
| Feature | Description | Priority |
|---------|-------------|----------|
| **Space Listing** | Add office photos, description, pricing | P1 |
| **Hardware Setup** | Link ESP32 device to listing | P1 |
| **Availability Calendar** | Set available hours/days | P1 |
| **Earnings Dashboard** | View revenue, pending settlements | P1 |
| **Withdraw Funds** | Transfer earnings to wallet | P1 |

### 5.2 Future Features (Nice-to-Have)

#### Phase 2:
- **Dynamic Pricing**: AI-based pricing based on demand
- **Group Bookings**: Multiple users sharing one space
- **Recurring Rentals**: Subscribe to regular time slots
- **Mobile App**: iOS/Android native apps
- **Multi-room Management**: One owner, multiple spaces

#### Phase 3:
- **Reviews & Ratings**: User reputation system
- **Dispute Resolution**: On-chain arbitration
- **Insurance Integration**: Cover damages/theft
- **Smart Contract Automation**: Auto-renewal, dynamic terms
- **Multi-chain Support**: Expand beyond Sui

---

## 6. Technical Specifications

### 6.1 Hardware Requirements

#### ESP32 Development Board:
- **Model**: ESP32-S3-DevKitC-1 or ESP32-C3-DevKitM-1
- **Flash**: 4MB minimum (8MB recommended)
- **RAM**: 512KB SRAM (S3 has 8MB PSRAM)
- **WiFi**: 802.11 b/g/n
- **Bluetooth**: BLE 5.0 (for NFC alternative)

#### Display:
- **Type**: 1.28" Round TFT LCD
- **Resolution**: 240x240 pixels
- **Driver**: GC9A01
- **Interface**: SPI
- **Touch**: CST816S capacitive touch controller

#### Lock Mechanism:
- **Type**: TBD (Solenoid, Servo, or Electric Strike)
- **Voltage**: 12V DC (typical)
- **Control**: Relay module via GPIO
- **Current**: 500mA-2A (depends on lock type)

#### Power Supply:
- **Input**: 110-240V AC
- **Output**: 5V/3A for ESP32 + 12V/2A for lock
- **Backup**: Optional battery for time keeping

### 6.2 Software Stack

#### Firmware (ESP32):
- **Framework**: Arduino IDE (for MVP), ESP-IDF (for production)
- **Libraries**:
  - `MicroSui`: Sui signature generation (ED25519)
  - `TFT_eSPI`: LCD display driver
  - `ArduinoJson`: JSON parsing
  - `WiFiManager`: WiFi configuration
  - `NTPClient`: Time synchronization
  - `Crypto`: Signature generation
- **Language**: C++

#### Smart Contracts (Sui Blockchain):
- **Language**: Move
- **Modules**:
  - `tunnel.move`: P2P payment channel
  - `rental.move`: Office rental logic
  - `access_control.move`: Lock authorization
- **Testing**: Move unit tests

#### Web Application:
- **Framework**: NextJS 14 (App Router)
- **Language**: TypeScript
- **Frontend**:
  - React 18
  - Tailwind CSS
  - Shadcn UI components
  - LVGL (for ESP32 UI design)
- **Backend API**:
  - NextJS API routes (serverless)
  - Vercel KV (Redis-compatible storage)
- **Blockchain Integration**:
  - `@mysten/sui.js`: Sui SDK
  - `@mysten/enoki`: Wallet abstraction
  - `@mysten/wallet-kit`: Wallet connect

#### Development Tools:
- **IDE**: VSCode
- **Version Control**: Git + GitHub
- **Deployment**:
  - Vercel (web app)
  - Sui Testnet → Mainnet (smart contracts)
- **Testing**:
  - Jest (unit tests)
  - Playwright (E2E tests)
  - Sui Move tests

### 6.3 Data Models

#### Tunnel Object (On-Chain):
```move
public struct Tunnel<phantom T> has key, store {
    id: UID,
    deposit: Balance<T>,
    partyA_deposit_amount: u64,  // Renter
    partyB_deposit_amount: u64,  // Owner
    partyA_public_key: vector<u8>,
    partyB_public_key: vector<u8>,
    initial_penalty_amount: u64,
    signature_type: u8,  // ED25519 or BLS12381
}
```

#### Rental Object (On-Chain):
```move
public struct Rental has key, store {
    id: UID,
    office_id: ID,
    tunnel_id: ID,
    renter: address,
    owner: address,
    start_time: u64,
    end_time: u64,
    price_per_minute: u64,
    status: u8,  // Active, Completed, Disputed
}
```

#### Office Listing (Off-Chain DB):
```typescript
interface OfficeListing {
  id: string;
  owner_address: string;
  name: string;
  description: string;
  photos: string[];
  location: {
    address: string;
    lat: number;
    lng: number;
  };
  pricing: {
    per_hour: number;  // in SUI
    per_minute: number;
    deposit_required: number;
  };
  hardware: {
    esp32_public_key: string;
    device_id: string;
  };
  availability: {
    hours: string;  // e.g., "9am-6pm"
    days: number[];  // [1,2,3,4,5] = Mon-Fri
  };
  status: "active" | "maintenance" | "inactive";
}
```

#### User Profile (Off-Chain DB):
```typescript
interface UserProfile {
  id: string;
  wallet_address: string;
  email: string;
  name: string;
  role: "renter" | "owner" | "admin";
  reputation: {
    rating: number;
    total_rentals: number;
    disputes: number;
  };
  created_at: number;
}
```

### 6.4 API Endpoints

#### Public Endpoints:
- `GET /api/offices` - List all available offices
- `GET /api/offices/:id` - Get office details
- `GET /api/health` - Health check

#### Authenticated Endpoints:
- `POST /api/bookings` - Create new booking
- `GET /api/bookings/:id` - Get booking details
- `POST /api/bookings/:id/unlock` - Generate unlock token
- `POST /api/bookings/:id/end` - End rental and settle
- `GET /api/user/rentals` - Get user's rental history
- `POST /api/sponsor` - Sponsor transaction via Enoki
- `POST /api/execute` - Execute sponsored transaction

#### Owner Endpoints:
- `POST /api/owner/offices` - Create new listing
- `PUT /api/owner/offices/:id` - Update listing
- `GET /api/owner/earnings` - View earnings
- `POST /api/owner/withdraw` - Withdraw funds

#### ESP32 Endpoints:
- `POST /api/device/verify` - Verify booking token
- `POST /api/device/log` - Submit access logs
- `GET /api/device/config` - Get device configuration

---

## 7. User Flows

### 7.1 Renter Flow: Book and Access Office

```
1. User opens web app → Browses office listings
2. Selects office → Views photos, pricing, availability
3. Chooses time slot (e.g., 2:00 PM - 4:00 PM, 2 hours)
4. Clicks "Book Now"
5. Web app calculates:
   - Total cost: $20 (2 hours × $10/hour)
   - Deposit required: $20 (same as rental cost)
   - Total payment: $40 (will get $20 back if no issues)
6. User authenticates via Enoki (email login)
7. Web app creates Tunnel transaction:
   - Renter deposits: $40 in SUI
   - Owner deposits: $20 (penalty if service not provided)
8. User confirms transaction → Blockchain processes
9. Booking confirmed! QR code generated
10. At 2:00 PM, user arrives at office
11. Scans QR code on ESP32 LCD screen OR opens app → "Unlock"
12. ESP32 verifies:
    - Booking exists on-chain
    - Current time is within rental window
    - Tunnel is active
13. Lock opens ✅
14. LCD shows: "Welcome! Time remaining: 2:00:00"
15. User works in office
16. At 3:47 PM, user finishes and clicks "End Rental" in app
17. ESP32 logs exit time with signature
18. Smart contract calculates:
    - Actual usage: 1 hour 47 minutes = $17.83
    - Refund to renter: $40 - $17.83 = $22.17
    - Payment to owner: $17.83 + $20 deposit back
19. Funds automatically distributed
20. Lock secures
```

### 7.2 Owner Flow: List Office and Earn

```
1. Owner registers on platform → Connects wallet
2. Clicks "List New Space"
3. Fills out form:
   - Upload photos
   - Add description
   - Set location
   - Set pricing ($10/hour)
   - Set availability (Mon-Fri, 9am-6pm)
4. Platform generates ESP32 configuration
5. Owner purchases ESP32 hardware kit (or receives from platform)
6. Follows setup guide:
   - Connect ESP32 to WiFi
   - Scan setup QR code to link device to listing
   - Install lock mechanism
   - Test unlock/lock cycle
7. Listing goes live
8. Renter books space
9. Smart contract creates Tunnel:
   - Owner deposits $20 (service guarantee)
   - Renter deposits $40
10. ESP32 automatically handles unlock/lock
11. After rental ends, settlement occurs:
    - Owner receives payment + deposit back
12. Owner views earnings dashboard
13. Withdraws funds to wallet when desired
```

### 7.3 Edge Case: Dispute Resolution

```
Scenario: Renter claims office was dirty/unusable

1. Renter clicks "Report Issue" during rental
2. Takes photos of issue
3. Refuses to sign exit time
4. Tunnel enters "Disputed" state
5. Platform admin is notified
6. Admin reviews evidence:
   - ESP32 logs
   - Renter's photos
   - Previous reviews
7. Admin makes decision:
   Option A: Renter is right → Full refund + owner penalty
   Option B: Owner is right → Normal settlement
   Option C: Split difference → Partial refund
8. Admin executes settlement transaction
9. Funds distributed according to decision
10. Both parties notified
```

---

## 8. Security & Trust

### 8.1 Security Measures

#### Hardware Security:
- **Private key storage**: ESP32 secure element or encrypted flash
- **Tamper detection**: IMU detects physical movement/removal
- **Firmware signing**: Only authenticated firmware can run
- **Secure boot**: Prevent unauthorized firmware flashing

#### Smart Contract Security:
- **Audited code**: Third-party security audit before mainnet
- **Time locks**: Prevent rapid exploit attacks
- **Emergency pause**: Admin can halt contracts in emergency
- **Access control**: Only authorized addresses can perform actions

#### User Security:
- **Wallet abstraction**: Enoki hides private keys from users
- **Session tokens**: Temporary access tokens for unlocking
- **Rate limiting**: Prevent spam/brute force attacks
- **2FA option**: Optional two-factor authentication

### 8.2 Trust Mechanisms

#### For Renters:
- **Escrow deposit**: Owner must deposit penalty amount
- **Automated refunds**: Smart contract ensures fair settlement
- **Dispute resolution**: Platform admin can intervene
- **Reviews**: See previous renter experiences

#### For Owners:
- **Deposit requirement**: Renters must deposit upfront
- **Penalty for damages**: Can claim from deposit if issues
- **Automated payments**: No manual invoicing needed
- **Hardware control**: Owner can manually override lock if needed

---

## 9. Pricing & Economics

### 9.1 User Pricing (Example)

#### For Renters:
- **Rental fee**: Set by owner (e.g., $10/hour)
- **Platform fee**: 5% of rental cost (e.g., $0.50/hour)
- **Deposit**: Equal to rental cost (refunded after)
- **Gas fees**: Covered by platform (Enoki gas pool)

**Example 2-hour rental:**
- Rental cost: $20
- Platform fee: $1
- Deposit: $20 (refunded)
- Total paid upfront: $41
- Refunded after: $20
- **Net cost: $21**

#### For Owners:
- **Listing fee**: Free (MVP)
- **Platform commission**: 5% of rental fees
- **Hardware cost**: $150 one-time (ESP32 + lock kit)
- **Deposit requirement**: $20 per booking (refunded after)

**Example earnings:**
- 2-hour rental at $10/hour = $20
- Platform fee (5%): -$1
- **Net earnings: $19**

### 9.2 Platform Business Model

#### Revenue Streams:
1. **Transaction fees**: 5% of each rental (split between renter + owner)
2. **Hardware sales**: Margin on ESP32 lock kits
3. **Premium listings**: Featured placement for owners
4. **Enterprise licensing**: B2B sales to property managers

#### Cost Structure:
- **Gas subsidies**: Enoki gas pool for user transactions
- **Infrastructure**: Vercel hosting, databases
- **Hardware**: ESP32 manufacturing/procurement
- **Support**: Customer service, dispute resolution
- **Marketing**: User acquisition

---

## 10. Success Metrics

### 10.1 MVP Success Criteria

#### Technical Milestones:
- ✅ ESP32 successfully generates signatures offline
- ✅ Tunnel smart contract deploys and functions correctly
- ✅ Lock actuates reliably (99%+ success rate)
- ✅ Web app booking flow completes end-to-end
- ✅ Time tracking accurate to within 1 second
- ✅ Settlement calculates correct amounts

#### Demo Milestones:
- ✅ 3D printed office model complete
- ✅ Lock installs and operates on model
- ✅ Mini figure demonstration video created
- ✅ Pitch deck finalized
- ✅ Live demo at hackathon/pitch event

### 10.2 Product Metrics (Post-MVP)

#### Growth Metrics:
- **Total users**: Target 100 users in first 3 months
- **Active listings**: Target 10 offices in first city
- **Bookings per month**: Target 50+ bookings/month
- **Repeat usage**: 30%+ of users book multiple times

#### Financial Metrics:
- **GMV (Gross Merchandise Value)**: Total rental fees processed
- **Revenue**: Platform fees collected
- **Hardware sales**: Number of lock kits sold

#### Technical Metrics:
- **Uptime**: 99.5%+ for web app and smart contracts
- **Lock reliability**: 99%+ successful unlock attempts
- **Settlement success**: 95%+ automatic settlements (no disputes)
- **Response time**: < 2 seconds for unlock verification

#### User Satisfaction:
- **NPS score**: Target 50+
- **Average rating**: 4.5/5 stars
- **Dispute rate**: < 5% of bookings
- **Churn rate**: < 10% monthly

---

## 11. Risks & Mitigations

### 11.1 Technical Risks

| Risk | Impact | Probability | Mitigation |
|------|--------|-------------|------------|
| **ESP32 connectivity loss** | High | Medium | Offline mode: ESP32 caches bookings locally |
| **Smart contract exploit** | Critical | Low | Security audit, bug bounty, gradual rollout |
| **Lock hardware failure** | High | Medium | Manual override for owner, backup battery |
| **Time sync issues** | Medium | Low | NTP sync every 10 minutes, blockchain timestamps |
| **Signature verification failure** | High | Low | Fallback to server-side verification |

### 11.2 Business Risks

| Risk | Impact | Probability | Mitigation |
|------|--------|-------------|------------|
| **Low user adoption** | High | Medium | Focus on one city, partner with coworking spaces |
| **Regulatory issues** | Critical | Low | Legal review, start in crypto-friendly jurisdictions |
| **Crypto UX barrier** | High | High | Wallet abstraction via Enoki, fiat on-ramps |
| **Competition from Web2 platforms** | Medium | High | Differentiate on fees and flexibility |
| **Hardware theft/vandalism** | Medium | Medium | Tamper detection, owner insurance option |

### 11.3 Operational Risks

| Risk | Impact | Probability | Mitigation |
|------|--------|-------------|------------|
| **Dispute escalation** | Medium | High | Clear ToS, admin arbitration process |
| **Support overhead** | Medium | High | Self-service help docs, community forum |
| **Physical property damage** | High | Low | Deposits cover damages, insurance partnerships |
| **Scalability bottleneck** | Medium | Medium | Modular architecture, horizontal scaling |

---

## 12. Development Roadmap

### 12.1 MVP Timeline (3-4 months)

#### Phase 1: Foundation (Weeks 1-4)
- [ ] Set up development environment
- [ ] ESP32 hardware selection and procurement
- [ ] Initial smart contract development (Tunnel module)
- [ ] Web app scaffolding (NextJS + Sui SDK)
- [ ] 3D model design for demo office

#### Phase 2: Core Development (Weeks 5-8)
- [ ] ESP32 firmware development:
  - MicroSui integration
  - LCD display drivers
  - WiFi connectivity
  - Lock control logic
- [ ] Smart contract development:
  - Complete Tunnel implementation
  - Rental logic
  - Settlement calculations
- [ ] Web app development:
  - User authentication (Enoki)
  - Booking flow UI
  - Payment integration
  - Unlock interface

#### Phase 3: Integration (Weeks 9-10)
- [ ] ESP32 ↔ Blockchain integration
- [ ] Web app ↔ Blockchain integration
- [ ] End-to-end testing
- [ ] 3D printing and assembly
- [ ] Lock installation on model

#### Phase 4: Polish & Demo (Weeks 11-12)
- [ ] Bug fixes and optimization
- [ ] Demo video production
- [ ] Pitch deck creation
- [ ] User documentation
- [ ] Hackathon/investor presentation prep

### 12.2 Post-MVP Roadmap

#### Q2 2026: Beta Launch
- Real-world pilot with 5 offices in one city
- Onboard first 50 users
- Gather feedback and iterate
- Security audit of smart contracts

#### Q3 2026: Public Launch
- Expand to 3 cities
- Mobile app release (React Native)
- Dynamic pricing features
- Partnerships with coworking spaces

#### Q4 2026: Scale
- 10+ cities, 100+ offices
- B2B partnerships with property managers
- White-label lock module sales
- Explore alternative verticals (parking, storage)

---

## 13. Alternative Use Cases / Pivot Options

The core technology (blockchain-controlled smart locks) can serve multiple markets:

### 13.1 Parking Spaces
**Why it's better than offices:**
- Simpler access control (barrier gate)
- Huge existing market ($50B+ globally)
- Less regulation than commercial offices
- Proven P2P models (SpotHero, ParkWhiz)

**Technical changes needed:**
- Barrier gate controller instead of door lock
- License plate recognition (optional)
- Faster unlock time (<5 seconds)

### 13.2 Storage Units
**Why it's attractive:**
- Lower value per transaction (less risk)
- Standardized units (easier to scale)
- Less regulation
- Recurring rental model

**Technical changes needed:**
- Padlock or roll-up door mechanism
- Monthly billing instead of hourly
- Climate monitoring (optional)

### 13.3 Equipment Rentals
**Why it's interesting:**
- High-value items (cameras, tools, bikes)
- NFT-based ownership proof
- Smart locks on equipment cases
- Insurance integration potential

**Technical changes needed:**
- Portable lock devices
- GPS tracking
- Damage detection sensors

### 13.4 EV Charging Stations
**Why it's promising:**
- Growing market (EV adoption)
- Clear time-based pricing
- Automated start/stop
- Government incentives

**Technical changes needed:**
- Charging controller integration
- Power metering
- Dynamic pricing based on electricity rates

---

## 14. Open Questions / Decisions Needed

### 14.1 Technical Decisions
- [ ] **Lock type**: Solenoid, servo, or electric strike?
- [ ] **Power source**: Hardwired or battery backup?
- [ ] **Communication protocol**: WiFi only or add BLE/NFC?
- [ ] **Signature algorithm**: ED25519 or BLS12381? (ED25519 recommended for speed)
- [ ] **Display interaction**: Touch-only or add physical buttons?

### 14.2 Business Decisions
- [ ] **Target market**: Start with offices or pivot to parking/storage?
- [ ] **Pricing model**: Fixed hourly rate or dynamic pricing?
- [ ] **Platform fee**: 5% or different structure?
- [ ] **Hardware sales**: Sell kits or provide free to early owners?
- [ ] **Geographic focus**: Which city for pilot?

### 14.3 Product Decisions
- [ ] **Web2 support**: How aggressive on fiat on-ramps?
- [ ] **Mobile app**: MVP or post-launch?
- [ ] **Admin role**: Fully decentralized or platform-managed disputes?
- [ ] **Multi-token**: SUI-only or add stablecoins immediately?

---

## 15. Resources & References

### 15.1 Technical Resources
- **MicroSui ESP32**: https://github.com/MicroSui/microsui-esp32
- **Tunnel (State Channels)**: /Users/nikola/Developer/arduino/esp32-c3-lcd28inch/.ref/tunnel-main
- **Sui Documentation**: https://docs.sui.io
- **ESP32 Documentation**: https://docs.espressif.com/projects/esp-idf

### 15.2 Hardware Suppliers
- **ESP32 Boards**: Adafruit, SparkFun, AliExpress
- **LCD Displays**: Waveshare, AliExpress
- **Lock Mechanisms**:
  - Solenoid locks: Adafruit, Amazon
  - Electric strikes: Locknetics, HES
  - Smart locks: August, Wyze (for reference)

### 15.3 Competitive Analysis
- **WeWork**: https://www.wework.com
- **LiquidSpace**: https://liquidspace.com
- **August Smart Locks**: https://august.com
- **SpotHero (parking)**: https://spothero.com

---

## 16. Appendix

### 16.1 Glossary
- **Tunnel**: P2P payment channel smart contract allowing off-chain state updates
- **MicroSui**: Library enabling ESP32 to generate Sui-compatible signatures
- **Enoki**: Sui's wallet abstraction service for simplified user onboarding
- **Move**: Sui's smart contract programming language
- **ESP32**: Low-cost microcontroller with WiFi and Bluetooth
- **State Channel**: Off-chain scaling solution for blockchains

### 16.2 Contact & Team
- **Project Lead**: [Your Name]
- **Technical Lead**: [Name]
- **Smart Contract Developer**: [Name]
- **Hardware Engineer**: [Name]

---

**Document Control:**
- **Last Updated**: November 1, 2025
- **Next Review**: December 1, 2025
- **Version History**:
  - v1.0 (Nov 1, 2025): Initial PRD creation

