cat << EOF >> nm80-dictionary-bot.sh
#!/bin/bash

# if not found - equals to 1, start it
while true
do
        ps -ef | grep nm80-dictionary-bot | grep -v grep | grep -v bash | grep -v nm80-dictionary-bot.sh
        if [ \$? -eq 1 ]
        then
                cd $PWD/build
                $PWD/build/nm80-dictionary-bot
        else
                echo "nm80-dictionary-bot is running"
        fi
        sleep 30
done
EOF

chmod +x nm80-dictionary-bot.sh
