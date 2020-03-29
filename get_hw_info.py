# Necessary libraries
import requests
import json
import time
import re
import os

def main( ):
    with open( 'login_data.json' ) as login_data:
        login = json.load( login_data )
        
    if not login:
        return
        
    if len( login[ 'Usuario' ] ) < 2:
        return
    
    s = requests.Session( )
    
    user_agent = 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.47 Safari/537.36'
    
    s.post( 'https://portal.p4ed.com/', data = login, headers = { 'User-Agent': user_agent } )
    
    resp = s.get( 'https://portal.p4ed.com/Login/AutenticacaoPortalEdrosAlunos/0' )
    
    login_token = re.search( r"'https:\/\/student\.p4ed\.com\/login\/loginbytoken\/(.+)?'", resp.text ).group( )
    login_token = login_token[ 1 : -1 ]

    s.get( login_token )
    
    resp = s.get( 'https://student.p4ed.com/tarefas/dados/2/0' )
    resp_json = json.loads( resp.text )
    
    for i in range( len( resp_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ] ) ):
        if len( resp_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ][ i ][ "Imagens" ] ):
            for j in range( len( resp_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ][ i ][ "Imagens" ] ) ):
                id = str( resp_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ][ i ][ "Imagens" ][ j ][ "Id" ] )
                if not os.path.exists( "images\\" + id + ".png" ):
                    img_resp = s.get( "https://student.p4ed.com/imagens2/obterimagemtarefa/" + id )
                    with open( "images\\" + id + ".png", 'wb' ) as outfile: 
                        outfile.write( img_resp.content )
    index = 1
    
    while( True ):
        resp = s.get( 'https://student.p4ed.com/tarefas/dados/2/' + str( index ) )
        
        cur_json = json.loads( resp.text )
        
        if len( cur_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ] ):
            for i in range( len( cur_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ] ) ):
                if len( cur_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ][ i ][ "Imagens" ] ):
                    for j in range( len( cur_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ][ i ][ "Imagens" ] ) ):
                        id = str( cur_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ][ i ][ "Imagens" ][ j ][ "Id" ] )
                        if not os.path.exists( "images\\" + id + ".png" ):
                            img_resp = s.get( "https://student.p4ed.com/imagens2/obterimagemtarefa/" + id )
                            with open( "images\\" + id + ".png", 'wb' ) as outfile: 
                                outfile.write( img_resp.content )
                    
                resp_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ].insert( index * 10 + i, cur_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ][ i ] )
        else:
            break
        
        index += 1
        
    index -= 1
    index2 = 0
    
    while( True ):
        resp = s.get( 'https://student.p4ed.com/tarefas/dados/3/' + str( index2 ) )
        
        cur_json = json.loads( resp.text )
        
        if ( len( cur_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ] ) ):
            for i in range( len( cur_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ] ) ):
                if len( cur_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ][ i ][ "Imagens" ] ):
                    for j in range( len( cur_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ][ i ][ "Imagens" ] ) ):
                        id = str( cur_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ][ i ][ "Imagens" ][ j ][ "Id" ] )
                        if not os.path.exists( "images\\" + id + ".png" ):
                            img_resp = s.get( "https://student.p4ed.com/imagens2/obterimagemtarefa/" + id )
                            with open( "images\\" + id + ".png", 'wb' ) as outfile: 
                                outfile.write( img_resp.content )
                            
                resp_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ].insert( index * 10 + i, cur_json[ "Dados" ][ "DadosPagina" ][ "Tarefas" ][ i ] )
        else:
            break
        
        index += 1
        index2 += 1

    with open( 'result_data.json', 'w+', encoding = 'utf8' ) as result_file:
        json.dump( resp_json, result_file )
        
    result_file.close( )

if __name__ == '__main__':
    main( )