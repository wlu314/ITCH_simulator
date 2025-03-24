#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>      // For memcpy, memset
#include <algorithm>    // For std::min, std::copy_n
#include <string>
#include <array>
#include <sstream>
#include <iomanip>

#include "message.hpp" // ITCH protocol message struct 
#include "constant.hpp" // ITCH constants

// Helper function: packs a 64-bit timestamp (with only the lower 48 bits valid)
// into a std::array<char, 6> in big-endian order
void packTimestamp(std::array<char, 6>& dest, uint64_t timestamp) {
    for (int i = 0; i < 6; ++i) {
        dest[i] = static_cast<char>((timestamp >> (8 * (5 - i))) & 0xFF);
    }
}

// Helper template to pack a string into a fixed-size std::array<char, N>
// Padding with spaces if needed.
template <size_t N>
void packString(std::array<char, N>& dest, const std::string &s) {
    std::fill(dest.begin(), dest.end(), ' ');
    size_t len = std::min(s.size(), static_cast<size_t>(N));
    std::copy_n(s.begin(), len, dest.begin());
}


// SystemEventMessage
std::vector<uint8_t> generateSystemEventMessage(
    uint16_t tracking_number, 
    uint64_t timestamp,
    char event_code
){
    SystemEventMessage msg{}; 
    msg.message_type = static_cast<char>(MessageType::SystemEvent);
    msg.stock_locate = 0;
    msg.tracking_number = tracking_number;
    packTimestamp(msg.timestamp, timestamp);
    msg.event_code = event_code;
    
    std::vector<uint8_t> message(sizeof(SystemEventMessage));
    std::memcpy(message.data(), &msg, sizeof(SystemEventMessage));
    return message;
}

// StockDirectoryMessage
std::vector<uint8_t> generateStockDirectoryMessage(
    uint16_t stock_locate,
    uint16_t tracking_number,
    uint64_t timestamp,
    const std::string &stock,
    char market_category,
    char financial_status_indicator,
    uint32_t round_lot_size,
    char round_lots_only,
    char issue_classification,
    const std::string &issue_subtype,
    char authenticity,
    char short_sale_threshold_indicator,
    char ipo_flag,
    char LULDReference_price_tier,
    char etp_flag,
    uint32_t etp_leverage_factor,
    char inverse_indicator)
{
    StockDirectoryMessage msg{};
    msg.message_type = static_cast<char>(MessageType::StockDirectory);
    msg.stock_locate = stock_locate; 
    msg.tracking_number = tracking_number;
    packTimestamp(msg.timestamp, timestamp);
    packString(msg.stock, stock);
    msg.market_category = market_category;
    msg.financial_status_indicator = financial_status_indicator;
    msg.round_lot_size = round_lot_size;
    msg.round_lots_only = round_lots_only;
    msg.issue_classification = issue_classification;

    std::fill(msg.issue_subtype.begin(), msg.issue_subtype.end(), ' ');
}



std::vector<uint8_t> generateAddOrderMessage(
    uint64_t timestamp,
    uint64_t orderRef,
    uint8_t side,
    uint32_t shares,
    const std::string &stockSymbol,
    uint32_t price
){
    AddOrderMessage msg{};
    msg.message_type = static_cast<char>(MessageType::AddOrder);
    msg.stock_locate = 0;
    msg.tracking_number = 0;
    for (int i = 0; i < 6; ++i) {
        msg.timestamp[i] = static_cast<char>((timestamp >> (8 * (5 - i))) & 0xFF);
    }
    msg.order_reference_number = orderRef;
    msg.side = static_cast<char>(side);
    msg.shares = shares;

    std::fill(msg.stock.begin(), msg.stock.end(), ' ');
    size_t len = std::min(stockSymbol.size(), static_cast<size_t>(msg.stock.size()));
    std::copy_n(stockSymbol.begin(), len, msg.stock.begin());

    msg.price = price;

    std::vector<uint8_t> message(sizeof(AddOrderMessage));
    std::memcpy(message.data(), &msg, sizeof(AddOrderMessage));
    

    return message;
}