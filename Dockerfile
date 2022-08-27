FROM reo7sp/tgbot-cpp

WORKDIR /usr/src/nm80-dictionary-bot
COPY . /usr/src/nm80-dictionary-bot/
RUN cd build/
RUN cmake ..
RUN make -j4
CMD ./nm80-dictionary-bot