### how to use
- install [C++ library for Telegram bot API](https://github.com/reo7sp/tgbot-cpp#dependencies)
- get [`LEARNER_WEBSTER_API`](https://dictionaryapi.com/products/api-learners-dictionary) and [`NM80_DICTIONARY_BOT_API`](https://core.telegram.org/bots) api tokens and set them in environmental varibles
- make this bot an admin of `log channel` and store this channel's id to env of `LOG_CHANNEL_ID`
- `git clone https://github.com/arashnm80/nm80-dictionary-bot`
- `cd nm80-dictionary-bot`
- `bash create-repeater-script.sh`
- `cd build`
- `cmake ..`
- `make`
- add `@reboot address-of-cloned-repo/nm80-dictionary-bot.sh` to crontab with `crontab -e` (change the address)
- reboot the system, now the bot should be up and running

**note:**
This is my first telegram bot and for now it's only supposed to work.
I'll do the improvements and modifications later.

**to-do:**
- add log channel to this bot.
