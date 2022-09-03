/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef FOXTROT_ACCOUNT_HPP
#define FOXTROT_ACCOUNT_HPP

#include "xengine.hpp"

using namespace xng;

class Account : public Messageable {
public:
    explicit Account(long balance = 0, long bankBalance = 0) : wallet(balance), bank(bankBalance) {}

    void addWallet(long value) {
        wallet += value;
    }

    void addBank(long value) {
        bank += value;
    }

    void moveToBank(long value) {
        if (value > wallet)
            throw std::runtime_error("Balance deduction would underflow balance.");
        wallet -= value;
        bank += value;
    }

    void moveToWallet(long value) {
        if (value > bank)
            throw std::runtime_error("Balance deduction would underflow balance.");
        bank -= value;
        wallet += value;
    }

    /**
     * Prioritize balance and pay remainder with bank.
     *
     * @param value
     */
    void deduct(long value) {
        if (value <= wallet) {
            wallet -= value;
        } else if (value > wallet - value + bank) {
            throw std::runtime_error("Balance deduction would underflow balance.");
        }
        bank -= value - wallet;
        wallet = 0;
    }

    void deductWallet(long value) {
        if (value > wallet)
            throw std::runtime_error("Balance deduction would underflow balance.");
        wallet -= value;
    }

    void deductBank(long value) {
        if (value > bank)
            throw std::runtime_error("Balance deduction would underflow balance.");
        bank -= value;
    }

    long getWalletBalance() const { return wallet; }

    long getBankBalance() const { return bank; }

    long getTotalBalance() const { return wallet + bank; }

    Messageable &operator<<(const Message &message) override {
        wallet = message.value("wallet", 0);
        bank = message.value("bank", 0);
        return *this;
    }

    Message &operator>>(Message &message) const override {
        message = Message(xng::Message::DICTIONARY);
        message["wallet"] = wallet;
        message["bank"] = bank;
        return message;
    }

private:
    long wallet;
    long bank;
};

#endif //FOXTROT_ACCOUNT_HPP
