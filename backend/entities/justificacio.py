from typing import Optional, Literal
from datetime import datetime, timezone
from sqlmodel import SQLModel, Field, Relationship
from typing import TYPE_CHECKING

from entities.tipus_justificacio import TipusJustificacioEnum

if TYPE_CHECKING:
    from entities.assistencia import Assistencia

class Justificacio(SQLModel, table=True):
    __tablename__ = "JUSTIFICACIONS"

    id: Optional[int] = Field(default=None, primary_key=True)
    assistencia_id: int = Field(foreign_key="ASSISTENCIA.id")
    motiu: str
    document: Optional[str] = None
    estat: TipusJustificacioEnum = TipusJustificacioEnum.pendiente
    data_solicitud: datetime = Field(default_factory=lambda: datetime.now(timezone))

    assistencia: "Assistencia" = Relationship(back_populates="justificacions")

from entities.assistencia import Assistencia