playshogi��2��usi�v���O�������m��ΐ킳���܂��B

1. 513��ɒB���������͎����I�Ɉ�������
2. �錾�����������I�Ɉ�������(�ʏ�͏����𖞂����Ă���"win"�𑗂�܂���
   �����𖞂�������w�����u�Ԃ�playshogi�����肵�܂�)�B27�_�@
3. AobaZero���g���ꍇ�A�����ɕ����̑ΐ�𑖂点�A�v���Z�X�ԃo�b�`��g�ނ��Ƃō������ł��܂��B
4. �����͕W���o�͂ɏo�܂��B
5. ��莁���쐬���ꂽ24��ڂ܂ł̌݊p��ՏW���g���đΐ�ł��܂��B
   http://yaneuraou.yaneu.com/2016/08/24/%E8%87%AA%E5%B7%B1%E5%AF%BE%E5%B1%80%E7%94%A8%E3%81%AB%E4%BA%92%E8%A7%92%E3%81%AE%E5%B1%80%E9%9D%A2%E9%9B%86%E3%82%92%E5%85%AC%E9%96%8B%E3%81%97%E3%81%BE%E3%81%97%E3%81%9F/
   records2016_10818.sfen
   ���J�����g�f�B���N�g���ɒu���ĉ������B
   ��Ղ͋N�����ƂɃ����_���ɑI�΂�܂��B



��:
AobaZero���m��ΐ킳����ꍇ�B800�ǁB�݊p��ՏW��400�ǎg���Đ����݂ɁB"-0"�����ł��B
./bin/playshogi -rsbm 800 -0 "./bin/aobaz -p 100 -w ./weight/w1198.txt" -1 "./bin/aobaz -p 100 -w ./weight/w1198.txt" >> w1198_p100_vs_w1198_p100.csa

AobaZero(1��800playout)��Kristallweizen(1��200k�m�[�h�A1�X���b�h�A��ՂȂ�)��ΐ킳����ꍇ�B�v���Z�X�ԃo�b�`���p�Bweight�̎w���playshogi�Aaobaz�A�������̂��w�肵�Ă�������(�����Ŏ��XGPU�̌v�Z��CPU�̌v�Z�̈�v���m�F���邽��)�B
./bin/playshogi -rsbm 600 -B 7 -P 18 -U 0 -c /bin/bash -W ./weight/w1198.txt -0 "./bin/aobaz -p 800 -e 0 -w ./weight/w1198.txt" -1 "~/Kristallweizen/yane483_nnue_avx2 usi , isready , setoption name BookMoves value 0 , setoption Threads value 1 , setoption NodesLimit value 200000" >> w1198_p800_vs_200k.csa

�� ����
ubuntu 16���� "-c /bin/bash" ��t���Ȃ���AobaZero�̃v���Z�X�ԃo�b�`�͓��삵�܂���B
CentOS���ƕK�v�Ȃ��ł��B����� "sh -c" �ŋN�������v���Z�X��ubuntu���Ǝq�v���Z�X�łȂ����v���Z�X�ɂȂ邽�߂ł��B



���ʂ̌���
   W-D-L    Games(DW-rep-DL) Sente WinR                WinRate 95%   ELO
   �� �� �s �ǐ� (�� �� ��)    ��菟��                 ����   95%   ELO
  437-13-350 800 (50-10-2)(s=422-365,0.536), m=133, wr=0.554(0.034)(  37)

  (50-10-2) �͐��̐錾������50�ǁA�����̈���������10�ǁA���̐錾������2�ǁA�ł��B
  513�蒴����13-10=3�ǂł��B








Usage: playshogi [OPTION] -0 "CMD0" -1 "CMD1"

      Generate gameplays between two USI shogi engines

Mandatory options:
  -0 "CMD0" Start player0 as '/bin/sh -c "CMD0"'.
  -1 "CMD1" Start player1 as '/bin/sh -c "CMD1"'.

Other options:
  -m NUM   Generate NUM gameplays. NUM must be a positive integer. The default
           value is 1.
  -f       Always assign player0 to Sente (black). If this is not specified,
           then Sente and Gote (white) are assigned alternatively.
  -r       Print CSA records.
  -s       Print results in detail.
  -u       Print verbose USI messages.
  -b       Use positions recorded in records2016_10818.sfen (a collection of
           24 moves from the no-handicap initial position).
  -c SHELL Use SHELL, e.g., /bin/csh, instead of /bin/sh.
  -P NUM   Generate NUM gameplays simultaneously. The default is 1.
  -I STR   Specifies nnet implementation. STR can conatin two characters
           separated by ':'. Character 'B' means CPU BLAS implementation, and
           'O' means OpenCL implimentation. The default is 0.
  -B STR   Specifies batch sizes of nnet computation. STR can contain two
           sizes separated by ':'. The default size is 1.
  -W STR   Specifies weight paths for nnet computation. STR can contain two
           file names separated by ':'.
  -U STR   Specifies device IDs of OpenCL nnet computation. STR can contain
           two IDs separated by ':'. Each ID must be different from the other.
  -H STR   OpenCL uses half precision floating-point values. STR can contain
           two binary values separated by ':'. The value should be 1 (use
           half) or 0 (do not use half). The default is 0.
  -T STR   Specifies the number of threads for CPU BLAS computation. STR can
           contain two numbers separated by ':'. The default is -1 (means an
           upper bound of the number).
Example:
  playshogi -0 "~/aobaz -w ~/w0.txt" -1 "~/aobaz -w ~/w1.txt"
           Generate a gameplay between 'w0.txt' (black) and 'w1.txt' (white)

