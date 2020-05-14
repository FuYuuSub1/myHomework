#!/usr/bin/bash

declare -a shortname
declare -a fullname
declare -a conference
declare -a region

declare -a win=( $(for i in {1..50}; do echo 0; done) )
declare -a lose=( $(for i in {1..50}; do echo 0; done) )
declare -a tie=( $(for i in {1..50}; do echo 0; done) )
declare -a pct=( $(for i in {1..50}; do echo 0; done) )
declare -a pf=( $(for i in {1..50}; do echo 0; done) )
declare -a pa=( $(for i in {1..50}; do echo 0; done) )
declare -a rank=( $(for i in {1..50}; do echo $i; done) )

declare -a filename=(`ls $2*`)


i=0
while IFS= read -r line || [ -n "$line" ]; do   
    c=0
    for word in $line; do
        case $c in
            0)
                shortname[i]=$word
                ;;
            1)
                fullname[i]=$word
                ;;
            2)
                conference[i]=$word
                ;;
            3)
                region[i]=$word
                ;;
        esac
        let c++
    done
    let i++
done < $1

for file in ${filename[*]}; do
    while read -r line || [ -n "$line" ]; do
        c=0
        for word in $line; do
            case $c in
                0)
                    for i in "${!shortname[@]}"; do
                        if [ $word == ${shortname[i]} ]; then
                            hometeam=$i
                        fi
                    done
                    ;;
                1)
                    scorehome=$word
                    ;;
                2)
                    for i in "${!shortname[@]}"; do
                        if [ $word == ${shortname[i]} ]; then
                            awayteam=$i
                        fi
                    done
                    ;;
                3)
                    scoreaway=$word
                    ;;
            esac
            let c++
        done
        pa[hometeam]=$((pa[hometeam]+scorehome))
        pa[awayteam]=$((pa[awayteam]+scoreaway))
        pf[hometeam]=$((pf[hometeam]+scoreaway))
        pf[awayteam]=$((pf[awayteam]+scorehome))
        if [ $scorehome -gt $scoreaway ]; then
            let win[hometeam]++
            let lose[awayteam]++
        elif [ $scorehome -lt $scoreaway ]; then
            let win[awayteam]++
            let lose[hometeam]++
        else
            let tie[hometeam]++
            let tie[awayteam]++
        fi
        
    done < $file
done

for i in "${!Sname[@]}"; do
    totalgame=$((win[i]+lose[i]+tie[i]))
    w=${win[i]}
    l=${lose[i]}
    t=${tie[i]}
    numerator=$(bc <<< "scale=2; $w+$t*0.5")
    pct[i]=$(bc <<< "scale=3; $numerator/$totalgame")
    net[i]=$((pf[i]-pa[i]))
done

counter=0
for i in "${!shortname[@]}"; do
    best=$i
    for j in "${!shortname[@]}"; do
        if [ $j -gt $i ]; then
            otherrank=${rank[j]}
            let otherresult=$((win[otherrank]-lose[otherrank]))

            betterrank=${rank[best]}
            let betterresult=$((win[betterrank]-lose[betterrank]))
            if [ $otherresult -gt $betterresult ]; then
                best=$j
            elif [ $otherresult -eq $betterresult ]; then
                if [ ${tie[otherrank]} -gt ${tie[betterrank]} ]; then
                    best=$j
                elif [ ${tie[otherrank]} -eq ${tie[betterrank]} ]; then
                    if [ ${net[otherrank]} -gt ${net[betterrank]} ]; then
                        largest=$j
                    fi
                fi
            fi
        fi
    done
    tmp=${rank[counter]}
    rank[counter]=${rank[best]}]
    rank[best]=$temp
    let counter++
done

target=0
declare -a inputconference
declare -a inputregion

inputcon=($(printf "%s\n" "${conference[@]}" | sort -u));
inputre=($(printf "%s\n" "${division[@]}" | sort -u));
no_con=0

if [ $# -gt 2 ]; then
    no_con=0
    counter2=1
    for item in $@; do
        if [ $counter2 -gt 2]; then
            case $no_con in
                0)
                    inputconference[$target]=$item
                    let no_con++
                    let counter++
                    ;;
                1)
                    inputregion[$target]=$item
                    let target++
                    let no_con++
                    let counter++
                    ;;
            esac
        fi
    done
else
    for item in "${!inputcon[@]}"; do
        inputconference[item]=${inputcon[i]}
    done
fi

thatteam=0

for i in "${!inputconference[@]}"; do
    counter3=1
    if [ $no_con -eq 0 ]; then
        printf "%-5s %-5s %-15s %-3s %-3s %-3s %-6s %-4s %-4s %-5s\n" ${inputconference[i]} ${inputregion[i]} Team W L T Pct PF PA Net
    else
        printf "%-10s %-15s %-3s %-3s %-3s %-6s %-4s %-4s %-5s\n" ${inputconference[i]} Team W L T Pct PF PA Net
    fi

    for k in "$!rank[@]"; do
        thatteam=${rank[k]}
        if [ ${inputconference[i]} == ${inputconference[thatteam]} ];then
            if [ $no_con -eq 2]; then
                printf "%-10d %-15s %-3d %-3d %-3d %-6.3f %-4d %-4d %-5d\n" $list ${fullname[thatteam]} ${win[thatteam]} ${lose[thatteam]} ${tie[thatteam]} ${pct[thatteam]} ${pf[thatteam]} ${pa[thatteam]} ${net[thatteam]}
                let counter3++
            elif [ $no_con -lt 2]; then
                if [ ${inputregion[i]} == ${division[thatteam]} ]; then
                    printf "%-11d %-15s %-3d %-3d %-3d %-6.3f %-4d %-4d %-5d\n" $list ${fullname[thatteam]} ${win[thatteam]} ${lose[thatteam]} ${tie[thatteam]} ${pct[thatteam]} ${pf[thatteam]} ${pa[thatteam]} ${net[thatteam]}
                    let counter++
                fi
            fi
        fi
    done
done