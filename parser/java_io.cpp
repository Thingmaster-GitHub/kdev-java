/*****************************************************************************
 * Copyright (c) 2005, 2006 Jakob Petsovits <jpetso@gmx.at>                  *
 *                                                                           *
 * This program is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU Library General Public               *
 * License as published by the Free Software Foundation; either              *
 * version 2 of the License, or (at your option) any later version.          *
 *                                                                           *
 * This grammar is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Lesser General Public License for more details.                           *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public License *
 * along with this library; see the file COPYING.LIB.  If not, write to      *
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
 * Boston, MA 02110-1301, USA.                                               *
 *****************************************************************************/

// This file is meant to be specific to the framework in which the parser
// operates, and is likely to be adapted for different environments.
// Specifically, the error output might not always go to qDebug(),
// but will rather be placed as items inside some listbox.


#include "javaparser.h"
#include "javalexer.h"
#include "parsesession.h"


#include <language/duchain/duchain.h>
#include <language/duchain/topducontext.h>
#include <language/duchain/duchainlock.h>
#include <language/editor/documentrange.h>

#include <language/duchain/problem.h>

#include <parsesession.h>

#include <iostream>


#include <QDebug>

// void print_token_environment(java::Parser* parser);
using namespace KDevelop;

namespace java
{

  void Parser::reportProblem(java::Parser *parser, Parser::ProblemType type, const QString& message )
  {

    IProblem::Severity severity;
    if (type == Error) {
      qDebug() << "** ERROR:" << message;
      severity = IProblem::Severity::Error;
    }
    else if (type == Warning) {
      qDebug() << "** WARNING:" << message;
      severity = IProblem::Severity::Warning;
    }
    else if (type == Info) {
      qDebug() << "** Info:" << message;
      severity = IProblem::Severity::Hint;
    }
    auto *p = new Problem();
    p->setSeverity(severity);
    p->setSource(IProblem::Parser);
    p->setDescription(message);
    KDevPG::TokenStream::TokenStreamBase::Token token = parser->m_session->tokenStream->curr();
    //data must be retrieved from the parser
    int sLine,sColumn,eLine,eColumn;
    CursorInRevision begin = parser->m_session->positionAt(token.begin);


    CursorInRevision end = parser->m_session->positionAt(token.end);

    sLine=begin.line;
    sColumn=begin.column;
    eLine=end.line;
    eColumn=end.column;
    p->setFinalLocation(DocumentRange(parser->m_document,
                                      KTextEditor::Range(sLine,sColumn,eLine,eColumn)));
    TopDUContext* top = DUChain::self()->chainForDocument(parser->m_document);
    if(top)
    {
      if(parser->m_session->firstError){
        parser->m_session->firstError=false;
        qDebug()<<"clearing problems";
        DUChainWriteLocker lock(DUChain::lock());
        top->clearProblems();
      }

      if(parser->m_problems.size()==0){
        qDebug()<<p;
        qDebug()<<"hey it ran!";
        DUChainWriteLocker lock(DUChain::lock());
        top->addProblem(ProblemPointer(p));
      }else{
        parser->m_problems.append(ProblemPointer(p));

        DUChainWriteLocker lock(DUChain::lock());
        while(parser->m_problems.size()){
          top->addProblem(parser->m_problems[0]);
        }
        parser->m_problems.clear();
      }
    }else{
      parser->m_problems.append(ProblemPointer(p));
      qDebug()<<"chain for "<<parser->m_document<<" not found!";
    }


  }


// custom error recovery
void Parser::expectedToken(int /*expected*/, qint64 /*where*/, const QString& name)
{
  // print_token_environment(this);
  reportProblem(this,
    Parser::Error,
    QString("Expected token ``%1''").arg(name)
      //+ QString(" instead of ``%1''").arg(current_token_text)
  );
}

void Parser::expectedSymbol(int /*expected_symbol*/, const QString& name)
{
  // print_token_environment(this);
  reportProblem(this,
    Parser::Error,
    QString("Expected symbol ``%1''").arg(name)
      //+ QString(" instead of ``%1''").arg(current_token_text)
  );
}

} // end of namespace java

